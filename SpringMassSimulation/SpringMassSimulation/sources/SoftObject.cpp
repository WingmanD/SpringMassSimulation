#include "SoftObject.h"

#include <glm/gtx/vector_angle.hpp>

#include "Globals.h"
#include "Particle.h"
#include "Util.h"


void SoftObject::draw(Camera* camera, Transform instanceTransform, bool selected) {
    Object::draw(camera, instanceTransform, selected);
    drawParticles(camera);
}

void SoftObject::tick(double deltaTime) {
    Object::tick(deltaTime);


    for (auto particle : particles) particle->applyPhysics(deltaTime);

    /*for (const auto value : particles)
        std::cout << "force: " << value->force.x << " " << value->force.y << " " << value->
            force.z << std::endl;*/

    

    for (const auto envForce : *environmentForces) envForce->apply(particles);
}

void SoftObject::drawParticles(Camera* camera) const {
    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(particleShader->ID);

    glm::mat4 perspective = camera->getProjectionMatrix();
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 model = getModelMatrix();

    glBindVertexArray(VAO_particles);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_particles);
    glBufferData(GL_ARRAY_BUFFER, flatParticles.size() * sizeof(Particle), flatParticles.data(), GL_DYNAMIC_DRAW);

    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, GL_FALSE, &perspective[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE, &model[0][0]);

    glDrawArrays(GL_POINTS, 0, flatParticles.size());

    glBindVertexArray(0);
}

void SoftObject::setupParticles() {
    for (const auto& vertex : mesh->vertices)
        flatParticles.emplace_back(
            Particle(vertex.position, particleMass, springConstant));

    for (auto& flatParticle : flatParticles)
        particles.emplace_back(&flatParticle);

    for (int i = 0; i < particles.size(); i++)
        for (const auto connected : mesh->vertices[i].connected)
            particles[i]->
                addConnected(particles[connected->index]);


    /*for (const auto& flat_particle : flatParticles)
        std::cout << "particle: " << flat_particle.position.x << " " <<
            flat_particle.position.y << " " << flat_particle.position.z << std::endl;
    for (const auto particle : particles)
        std::cout << "pointer particles " << particle->position.x << " " << particle->
                                                                            position.y << " " << particle->position.z <<
            std::endl;*/
    // tick(0.8);
}

void SoftObject::initParticleBuffer() {
    glGenVertexArrays(1, &VAO_particles);
    glGenBuffers(1, &VBO_particles);

    glBindVertexArray(VAO_particles);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_particles);
    glBufferData(GL_ARRAY_BUFFER, flatParticles.size() * sizeof(Particle), flatParticles.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle),
                          reinterpret_cast<void*>(offsetof(Particle, position)));

    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}
