#include "forces/RigidBodyCollisionForce.h"

#include <glm/geometric.hpp>
#include <glm/ext/matrix_projection.hpp>
#include <glm/ext/quaternion_geometric.hpp>

RigidBodyCollisionForce::RigidBodyCollisionForce(glm::vec3 position, glm::vec3 direction): position(position),
    direction(normalize(direction)) {
    D = -dot(direction, position);
}

void RigidBodyCollisionForce::apply(std::vector<Particle*> particles) {
    for (const auto particle : particles) {
        float distance = dot(particle->position, direction) + D;

        // if particle is a little above the plane, push it away with the same force that it pushes the plane 
        if (distance < 0.001f) {
            float forceProjectionMagnitude = dot(particle->force, -direction);
            particle->force += direction * forceProjectionMagnitude;

            float velocityProjectionMagnitude = dot(particle->velocity, -direction);
            particle->velocity += direction * velocityProjectionMagnitude;
        }
        
        // if particle is under the plane, teleport it on it's surface
        if (distance < 0) {
            particle->position += direction * -distance;
            particle->velocity = glm::vec3(0);
        }
    }
}

void RigidBodyCollisionForce::update() {}
RigidBodyCollisionForce::~RigidBodyCollisionForce() {}
