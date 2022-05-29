#include "SoftObject.h"

#include <thread>
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

    for (auto envForce : *environmentForces) envForce->apply(particles);

    for (auto particle : particles) particle->applyPhysics(deltaTime);
}

void SoftObject::drawParticles(Camera* camera) {
    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(particleShader->ID);

    glm::mat4 perspective = camera->getProjectionMatrix();
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 model = getModelMatrix();

    glBindVertexArray(VAO_particles);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_particles);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), &particles.front(), GL_DYNAMIC_DRAW);

    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, GL_FALSE, &perspective[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE, &model[0][0]);

    glDrawArrays(GL_POINTS, 0, particles.size());

    glBindVertexArray(0);
}

void SoftObject::initParticles() {


    glGenVertexArrays(1, &VAO_particles);
    glGenBuffers(1, &VBO_particles);

    glBindVertexArray(VAO_particles);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_particles);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), &particles.front(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle),
                          reinterpret_cast<void*>(offsetof(Particle, position)));

    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

}
