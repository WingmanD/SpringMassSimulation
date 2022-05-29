#include "forces/RigidBodyCollisionForce.h"

#include <glm/geometric.hpp>

RigidBodyCollisionForce::RigidBodyCollisionForce(glm::vec3 position, glm::vec3 direction) {
    this->position = position;
    this->direction = glm::normalize(direction);
    D = -dot(direction, position);
}

void RigidBodyCollisionForce::apply(std::vector<Particle*> particles) {
    for (auto particle : particles) {
        float distance = glm::abs(dot(particle->position, direction) + D);
        particle->force += direction * distance;
    }
}

void RigidBodyCollisionForce::update() {}
RigidBodyCollisionForce::~RigidBodyCollisionForce() {}
