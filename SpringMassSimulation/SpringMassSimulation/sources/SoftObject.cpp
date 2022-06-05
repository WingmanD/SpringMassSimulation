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


    /*for (auto particle : particles) particle->calculateSpringForces();

    for (const auto value : particles)
        std::cout << "springForce: " << value->force.x << " " << value->force.y << " " << value->
            force.z << std::endl;*/

    //todo check why calculateForces->applyEnvForces->applyPhysics does not work
    for (auto particle : particles) particle->applyPhysics(static_cast<float>(deltaTime));

    //for (const auto object : *otherObjectsInScene) if (object->bHasCollision) collide(object);

    for (const auto envForce : *environmentForces) envForce->apply(particles);
    /*for (const auto value : particles)
        std::cout << "envForces: " << value->force.x << " " << value->force.y << " " << value->
            force.z << std::endl;*/

    for (int i = 0; i < mesh->vertices.size(); ++i) { mesh->vertices[i].position = particles[i]->position; }

    // recalculate normals
    auto particle = particles.begin();
    for (const auto normal : mesh->calculateNormals()) (*particle++)->normal = normal;

}

void SoftObject::appliedTranslate(glm::vec3 newLocation) {
    glm::vec3 center = glm::vec3(0);
    for (auto& vertex : mesh->vertices) center += vertex.position;
    center /= mesh->vertices.size();

    const glm::vec3 offset = newLocation - center;

    for (auto& vertex : mesh->vertices) vertex.position += offset;
    for (auto particle : particles) particle->position += offset;
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

    for (auto particle : particles) {
        //if (particle != particles.front()) break;
        float minAbsDistance = std::numeric_limits<float>::max();
        float minDistance = std::numeric_limits<float>::max();
        glm::vec3 closestNormal = glm::vec3(0);

        // find triangle that is closest to particle and particle is above or below it
        for (auto triangle : object->mesh->triangles) {
            auto normal = triangle.getPhysicalNormal();

            //if (abs(dot(normal, particle->position - triangle.getCenter())) < 0.0001) continue;

            float D = -dot(triangle.v0->position, normal);
            float distance = dot(normal, particle->position) + D;

            glm::vec3 P = particle->position + distance * -normal;

            glm::vec3 n0 = normalize(cross(triangle.v1->position - triangle.v0->position, P - triangle.v0->position));
            glm::vec3 n1 = normalize(cross(triangle.v2->position - triangle.v1->position, P - triangle.v1->position));
            glm::vec3 n2 = normalize(cross(triangle.v0->position - triangle.v2->position, P - triangle.v2->position));


            const float threshold = 0.01f;
            if (!(abs(abs(dot(n0, n1) + dot(n0, n2) + dot(n1, n2)) - 3) < threshold)) {
                //std::cout << "outside" << std::endl;
                continue;
            }


            /*glm::vec3 e1 = triangle.v2->position - triangle.v1->position;
            glm::vec3 v1P = P - triangle.v1->position;
            glm::vec3 C = cross(e1, v1P);
            if (dot(C, normal) < 0) continue;

            glm::vec3 e2 = triangle.v0->position - triangle.v2->position;
            glm::vec3 v2P = P - triangle.v2->position;
            C = cross(e2, v2P);
            if (dot(C, normal) < 0) continue;*/

            // particle is directly above or below the triangle (it's projection is inside the triangle) -> calculate possible collision

            float absDistance = abs(distance);
            if (absDistance < minAbsDistance) {
                /*std::cout << "normals: " << n0.x << " " << n0.y << " " << n0.z << std::endl;
                std::cout << "        " << n1.x << " " << n1.y << " " << n1.z << std::endl;
                std::cout << "        " << n2.x << " " << n2.y << " " << n2.z << std::endl;
                std::cout << "dot 01, 02, 12: " << dot(n0, n1) << " " << dot(n0, n2) << " " << dot(n1, n2) << std::endl;

                std::cout << "particle position: " << particle->position.x << " " << particle->position.y << " " <<
                    particle->position.z << std::endl;
                std::cout << "projection: " << P.x << " " << P.y << " " << P.z << std::endl;
                std::cout << "normal: " << normal.x << " " << normal.y << " " << normal.z << std::endl;
                std::cout << "distance: " << distance << std::endl;
                std::cout << "vertices: " << triangle.v0->position.x << " " << triangle.v0->position.y << " " <<
                    triangle.v0->position.z << std::endl;
                std::cout << "          " << triangle.v1->position.x << " " << triangle.v1->position.y << " " <<
                    triangle.v1->position.z << std::endl;
                std::cout << "          " << triangle.v2->position.x << " " << triangle.v2->position.y << " " <<
                    triangle.v2->position.z << std::endl;
                std::cout << "abs: " << abs(abs(dot(n0, n1) + dot(n0, n2) + dot(n1, n2)) - 3) << std::endl;
                */


                /*if ((dot(n0, n1) - 1.0f) > threshold || (dot(n0, n2) - 1.0f) > threshold || (dot(n1, n2) - 1.0f) >
                    threshold) {
                    std::cout << "outside" << std::endl;
                    break;;
                }*/
                //std::cout << "INSIDE" << std::endl;
                minAbsDistance = absDistance;
                minDistance = distance;
                closestNormal = normal;

                /*std::cout << "projection: " << P.x << " " << P.y << " " << P.z << std::endl;
                std::cout << "normal: " << normal.x << " " << normal.y << " " << normal.z << std::endl;
                std::cout << "distance: " << distance << std::endl;
                std::cout << "vertices: " << triangle.v0->position.x << " " << triangle.v0->position.y << " " <<
                    triangle.v0->position.z << std::endl;
                std::cout << "          " << triangle.v1->position.x << " " << triangle.v1->position.y << " " <<
                    triangle.v1->position.z << std::endl;
                std::cout << "          " << triangle.v2->position.x << " " << triangle.v2->position.y << " " <<
                    triangle.v2->position.z << std::endl;

                std::cout << "normals: " << n0.x << " " << n0.y << " " << n0.z << std::endl;
                std::cout << "        " << n1.x << " " << n1.y << " " << n1.z << std::endl;
                std::cout << "        " << n2.x << " " << n2.y << " " << n2.z << std::endl;*/
            }
        }

        particle->color = glm::vec3(1, 0, 0);
        // if particle is slightly above triangle -> add force to push it away
        if (minDistance > 0 && minDistance < 0.001f) {
            /*std::cout << "collision distance: " << minDistance << " particle: " << particle->position.x << " " <<
                particle->
                position.y << " " << particle->position.z << " triangle normal: " << closestNormal.x << " " <<
                closestNormal.y <<
                " " << closestNormal.z << std::endl;*/
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
            /*std::cout << "position after" << particle->position.x << " " << particle->position.y << " " << particle->
                position.z << std::endl;*/
            particle->velocity = glm::vec3(0);
            break;
        }
    }
}

void SoftObject::setupParticles() {
    flatParticles.reserve(mesh->vertices.size() + 1);
    particles.reserve(mesh->vertices.size() + 1);

    for (const auto& vertex : mesh->vertices)
        flatParticles.emplace_back(
            Particle(mesh, vertex.position, vertex.normal, particleMass, &springConstant, &internalSpringConstant, &damping,
                     &internalPressureForceConstant));

    for (int i = 0; i < flatParticles.size() - 1; i++)
        for (const auto connected : mesh->vertices[i].connected)
            flatParticles[i].
                addConnected(&flatParticles[connected->index]);

    auto center = mesh->getCenter();
    
    flatParticles.emplace_back(Particle(mesh, center, {0, 0, 0}, particleMass, &springConstant, &internalSpringConstant,
                                        &damping, &internalPressureForceConstant));

    auto centerParticle = &flatParticles.back();

    for (int i = 0; i < flatParticles.size() - 1; i++) {
        centerParticle->addConnected(&flatParticles[i]);
        flatParticles[i].addInnerConnected(centerParticle);
    }

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
