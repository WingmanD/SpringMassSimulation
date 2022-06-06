#include "Particle.h"

#include <glm/ext/quaternion_geometric.hpp>

#include "forces/Force.h"

#define R 8.314f
#define T 298.15f

void Particle::addConnected(Particle* connected) {
    initialDistancesToConnected->emplace_back(connected, distance(position, connected->position));
}

void Particle::addInnerConnected(Particle* innerConnected) {
    initialDistancesToInnerConnected->emplace_back(innerConnected, distance(position, innerConnected->position));
}


void Particle::applyPhysics(float deltaTime) {
    calculateSpringForces();
    if (*internalPressureConstant > 0.00001f) calculateInternalPressureForce(parentMesh->calculateSurfaceArea(),
                                                                      parentMesh->calculateVolume());
    calculateDampingForces();
    applyForce(deltaTime);
}

inline void Particle::calculateSpringForces() {
    // calculate spring force that every neighbour applies on this particle
    for (auto [connectedParticle, initialDistance] : *initialDistancesToConnected) {
        const float deltaX = initialDistance - distance(position, connectedParticle->position);
        // Fs = k * deltaX * 0.5 where k is spring constant and deltaX is the difference between the initial distance and the current distance as vector
        force += *k * deltaX * normalize(position - connectedParticle->position) * 0.5f;
    }
    
    // calculate spring force that every inner particle applies on this particle
    for (auto [connectedParticle, initialDistance] : *initialDistancesToInnerConnected) {
        const float deltaX = initialDistance - distance(position, connectedParticle->position);
        // Fs = k * deltaX * 0.5 where k is spring constant and deltaX is the difference between the initial distance and the current distance as vector
        force += *k_inner * deltaX * normalize(position - connectedParticle->position) * 0.5f;
    }
}

inline void Particle::calculateInternalPressureForce(float meshSurfaceArea, float meshVolume) {
    // Fp = normal * SnRT / V
    force += normal * meshSurfaceArea * *internalPressureConstant * R * T / meshVolume;
}

inline void Particle::calculateDampingForces() { force -= *c * velocity; }


void Particle::applyForce(float deltaTime) {
    // F = m * a
    // a = F / m
    const glm::vec3 acceleration = force / *mass;
    // v = v0 + a * t
    velocity += acceleration * deltaTime;
    
    // clamp velocity to 10
    if (length(velocity) > 10.0f)
        velocity = normalize(velocity) * 10.0f;

    // x = x0 + v * t
    position += velocity * deltaTime;

    // reset force for next iteration
    force = glm::vec3(0.0f);
}
