#include "forces/RigidBodyCollisionForce.h"

#include <glm/geometric.hpp>
#include <glm/ext/matrix_projection.hpp>

RigidBodyCollisionForce::RigidBodyCollisionForce(glm::vec3 position, glm::vec3 direction) {
    this->position = position;
    this->direction = normalize(direction);
    D = -dot(direction, position);
}

void RigidBodyCollisionForce::apply(std::vector<Particle*> particles) {
    for (auto particle : particles) {
        float distance = dot(particle->position, direction) + D;

        if (distance < 0.001f) {
            float forceProjectionMagnitude = dot(particle->force, -direction);
            particle->force += direction * forceProjectionMagnitude;

            float velocityProjectionMagnitude = dot(particle->velocity, -direction);
            particle->velocity += direction * velocityProjectionMagnitude;
        }
        if (distance < 0) {
            particle->position += direction * -distance;
            particle->velocity = glm::vec3(0);
        }
    }
}

void RigidBodyCollisionForce::update() {}
RigidBodyCollisionForce::~RigidBodyCollisionForce() {}
