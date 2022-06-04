#include "Particle.h"

#include <glm/ext/quaternion_geometric.hpp>

#include "forces/Force.h"

void Particle::addConnected(Particle* connected) {
    initialDistancesToConnected->emplace_back(connected, distance(position, connected->position));
}

void Particle::addInnerConnected(Particle* innerConnected) {
    initialDistancesToInnerConnected->emplace_back(innerConnected, distance(position, innerConnected->position));
}


void Particle::applyPhysics(float deltaTime) {
    calculateSpringForces();
    calculateInternalPressureForce();
    calculateDampingForces();
    applyForce(deltaTime);
}

inline void Particle::calculateSpringForces() {
    for (auto initDistConnected : *initialDistancesToConnected) {
        const float deltaX = initDistConnected.second - distance(position, initDistConnected.first->position);
        // Fs = k * deltaX * 0.5 where k is spring constant and deltaX is the difference between the initial distance and the current distance as vector
        force += *k * deltaX * normalize(position - initDistConnected.first->position) * 0.5f;
    }

    for (auto initDistInnerConnected : *initialDistancesToInnerConnected) {
        const float deltaX = initDistInnerConnected.second - distance(position, initDistInnerConnected.first->position);
        // Fs = k * deltaX * 0.5 where k is spring constant and deltaX is the difference between the initial distance and the current distance as vector
        force += *k_inner * deltaX * normalize(position - initDistInnerConnected.first->position) * 0.5f;
    }
}

inline void Particle::calculateInternalPressureForce() {
    if (!initialDistancesToInnerConnected->empty() && length(force) > 0.1f) {
        const Particle* center = (*initialDistancesToInnerConnected->begin()).first;
        const float currentDistance = distance(position, center->position);
        const float initialDistance = (*initialDistancesToInnerConnected->begin()).second;

        force += *internalPressureConstant * normal * (initialDistance / currentDistance) * initialDistance;
    }
}

inline void Particle::calculateDampingForces() { force -= *c * velocity; }


void Particle::applyForce(float deltaTime) {
    // F = m * a
    // a = F / m
    const glm::vec3 acceleration = force / mass;
    // v = v0 + a * t
    velocity += acceleration * deltaTime;
    if (length(velocity) > 10.0f)
        velocity = normalize(velocity) * 10.0f;

    // x = x0 + v * t
    position += velocity * deltaTime;

    // reset force for next iteration
    force = glm::vec3(0.0f);
}
