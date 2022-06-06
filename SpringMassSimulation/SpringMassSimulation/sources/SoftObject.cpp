#include "SoftObject.h"

#include <glm/gtx/vector_angle.hpp>

#include "Globals.h"
#include "Particle.h"
#include "Util.h"


void SoftObject::drawFirstPass(Camera* camera, Transform instanceTransform, std::vector<DirectionalLight*>& lights) {
    Object::drawFirstPass(camera, instanceTransform, lights);
}

void SoftObject::drawSecondPass(Camera* camera, Transform instanceTransform, std::vector<DirectionalLight*>& lights) {
    if (bShowParticles) drawParticles(camera);
}

void SoftObject::tick(double deltaTime) {
    Object::tick(deltaTime);
    
    for (const auto particle : particles) particle->applyPhysics(static_cast<float>(deltaTime));

    // collide with other objects
    //for (const auto object : *otherObjectsInScene) if (object->bHasCollision) collide(object);

    // apply environment forces
    for (const auto envForce : *environmentForces) envForce->apply(particles);

    // update vertex positions
    for (int i = 0; i < mesh->vertices.size(); ++i) { mesh->vertices[i].position = particles[i]->position; }

    // recalculate normals for internal pressure force for next iteration
    auto particle = particles.begin();
    for (const auto normal : mesh->calculateNormals()) (*particle++)->normal = normal;
}

void SoftObject::appliedTranslate(glm::vec3 newLocation) {
    glm::vec3 center = glm::vec3(0);
    for (auto& vertex : mesh->vertices) center += vertex.position;
    center /= mesh->vertices.size();

    const glm::vec3 offset = newLocation - center;

    for (auto& vertex : mesh->vertices) vertex.position += offset;
    for (const auto particle : particles) particle->position += offset;
}

void SoftObject::drawParticles(Camera* camera) const {
    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(particleShader->ID);

    glBindVertexArray(VAO_particles);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_particles[0]);
    glBufferData(GL_ARRAY_BUFFER, flatParticles.size() * sizeof(Particle), flatParticles.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_particles[1]);
    glBufferData(GL_ARRAY_BUFFER, flatParticles.size() * sizeof(Particle), flatParticles.data(), GL_DYNAMIC_DRAW);

    glUniformMatrix4fv(glGetUniformLocation(particleShader->ID, "projection"), 1, GL_FALSE,
                       &camera->getProjectionMatrix()[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(particleShader->ID, "view"), 1, GL_FALSE, &camera->getViewMatrix()[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(particleShader->ID, "model"), 1, GL_FALSE, &getModelMatrix()[0][0]);

    glDrawArrays(GL_POINTS, 0, flatParticles.size());

    glBindVertexArray(0);
}

void SoftObject::collide(Object* object) const {
    if (object == this) return;

    for (const auto particle : particles) {
        float minAbsDistance = std::numeric_limits<float>::max();
        float minDistance = std::numeric_limits<float>::max();
        glm::vec3 closestNormal = glm::vec3(0);

        // find triangle that is closest to particle and particle is above or below it
        for (auto triangle : object->mesh->triangles) {
            auto normal = triangle.getPhysicalNormal();

            float D = -dot(triangle.v0->position, normal);
            float distance = dot(normal, particle->position) + D;

            glm::vec3 P = particle->position + distance * -normal;

            glm::vec3 n0 = normalize(cross(triangle.v1->position - triangle.v0->position, P - triangle.v0->position));
            glm::vec3 n1 = normalize(cross(triangle.v2->position - triangle.v1->position, P - triangle.v1->position));
            glm::vec3 n2 = normalize(cross(triangle.v0->position - triangle.v2->position, P - triangle.v2->position));


            const float threshold = 0.01f;
            if (!(abs(abs(dot(n0, n1) + dot(n0, n2) + dot(n1, n2)) - 3) < threshold)) continue;
            
            // particle is directly above or below the triangle (it's projection is inside the triangle) -> calculate possible collision
            float absDistance = abs(distance);
            if (absDistance < minAbsDistance) {
                minAbsDistance = absDistance;
                minDistance = distance;
                closestNormal = normal;
            }
        }

        particle->color = glm::vec3(1, 0, 0);
        // if particle is slightly above triangle -> add force to push it away
        if (minDistance > 0 && minDistance < 0.001f) {
            float forceProjectionMagnitude = dot(particle->force, -closestNormal);
            particle->force += closestNormal * forceProjectionMagnitude * 0.5f;

            float velocityProjectionMagnitude = dot(particle->velocity, -closestNormal);
            particle->velocity += closestNormal * velocityProjectionMagnitude;
            break;
        }

        // if particle is below triangle -> teleport it into the triangle,
        // we don't want particles inside meshes because they can fall through the floor
        // and cause weird behaviour when colliding with meshes 
        if (minDistance < 0) {
            std::cout << "under " << minDistance << " normal " << closestNormal[0] << " " << closestNormal[1] << " " <<
                closestNormal[2] << " position before" << particle->position.x << " " << particle->position.y << " " <<
                particle->
                position.z << std::endl;
            particle->position += closestNormal * -minDistance;
            particle->velocity = glm::vec3(0);
            break;
        }
    }
}

void SoftObject::setupParticles() {
    // flatParticles is just used for rendering
    // particles is used for physics, it contains pointers to particles in flatParticles
    flatParticles.reserve(mesh->vertices.size() + 1);
    particles.reserve(mesh->vertices.size() + 1);

    // for each vertex in mesh, create a particle
    for (const auto& vertex : mesh->vertices)
        flatParticles.emplace_back(
            Particle(mesh, vertex.position, vertex.normal, &particleMass, &springConstant, &internalSpringConstant, &damping,
                     &internalPressureForceConstant));

    // for each vertex, find it's neighbours and link those particles
    for (int i = 0; i < flatParticles.size() - 1; i++)
        for (const auto connected : mesh->vertices[i].connected)
            flatParticles[i].
                addConnected(&flatParticles[connected->index]);

    // add a center particle
    auto center = mesh->getCenter();
    flatParticles.emplace_back(Particle(mesh, center, {0, 0, 0}, &particleMass, &springConstant, &internalSpringConstant,
                                        &damping, &internalPressureForceConstant));
    auto centerParticle = &flatParticles.back();

    // connect the center particle to all other particles
    for (int i = 0; i < flatParticles.size() - 1; i++) {
        centerParticle->addConnected(&flatParticles[i]);
        flatParticles[i].addInnerConnected(centerParticle);
    }
    // fill particles with pointers to particles in flatParticles
    for (auto& particle : flatParticles)
        particles.emplace_back(&particle);
}

void SoftObject::initParticleBuffer() {
    glGenVertexArrays(1, &VAO_particles);
    glGenBuffers(2, VBO_particles);

    glBindVertexArray(VAO_particles);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_particles[0]);
    glBufferData(GL_ARRAY_BUFFER, flatParticles.size() * sizeof(Particle), flatParticles.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle),
                          reinterpret_cast<void*>(offsetof(Particle, position)));

    glBindBuffer(GL_ARRAY_BUFFER, VBO_particles[1]);
    glBufferData(GL_ARRAY_BUFFER, flatParticles.size() * sizeof(Particle), flatParticles.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle),
                          reinterpret_cast<void*>(offsetof(Particle, color)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}
