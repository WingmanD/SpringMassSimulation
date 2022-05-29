#include "forces/RigidBodyCollisionForce.h"

#include <glm/geometric.hpp>
#include <glm/ext/matrix_projection.hpp>

RigidBodyCollisionForce::RigidBodyCollisionForce(glm::vec3 position, glm::vec3 direction) {
    this->position = position;
    this->direction = glm::normalize(direction);
    D = -dot(direction, position);
}

void RigidBodyCollisionForce::apply(std::vector<Particle*> particles) {
    for (auto particle : particles) {
        float distance = dot(particle->position, direction) + D;
        if (distance < 0.001f) {
            float forceProjectionMagnitude = glm::dot(particle->force, -direction);
            particle->force += direction * forceProjectionMagnitude;

            float velocityProjectionMagnitude = glm::dot(particle->velocity, -direction);
            particle->velocity += direction * velocityProjectionMagnitude;
        }
    }
}

void RigidBodyCollisionForce::update() {}
RigidBodyCollisionForce::~RigidBodyCollisionForce() {}
