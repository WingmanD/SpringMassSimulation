#include "Particle.h"

#include <glm/ext/quaternion_geometric.hpp>

#include "forces/Force.h"

void Particle::addConnected(Particle* connected) {
    initialDistancesToConnected.emplace(connected, distance(position, connected->position));
}

void Particle::addInnerConnected(Particle* innerConnected) {
    initialDistancesToInnerConnected.emplace(innerConnected, distance(position, innerConnected->position));
}


void Particle::applyPhysics(float deltaTime) {
    calculateSpringForces();
    calculateInternalPressureForce();
    calculateDampingForces();
    applyForce(deltaTime);
}

inline void Particle::calculateSpringForces() {
    for (auto initDistConnected : initialDistancesToConnected) {
        float deltaX = initDistConnected.second - distance(position, initDistConnected.first->position);
        // Fs = k * deltaX * 0.5 where k is spring constant and deltaX is the difference between the initial distance and the current distance as vector
        glm::vec3 springForce = *k * deltaX * normalize(position - initDistConnected.first->position) * 0.5f;
        force += springForce;
    }

    for (auto initDistInnerConnected : initialDistancesToInnerConnected) {
        float deltaX = initDistInnerConnected.second - distance(position, initDistInnerConnected.first->position);
        // Fs = k * deltaX * 0.5 where k is spring constant and deltaX is the difference between the initial distance and the current distance as vector
        glm::vec3 springForce = *k_inner * deltaX * normalize(position - initDistInnerConnected.first->position) * 0.5f;
        force += springForce;
    }
}

inline void Particle::calculateInternalPressureForce() {
    //todo we need to rotate normals
    if (!initialDistancesToInnerConnected.empty() && length(force)>0.1f) {
        Particle* center = (*initialDistancesToInnerConnected.begin()).first;
        const float currentDistance = distance(position, center->position);
        const float initialDistance = (*initialDistancesToInnerConnected.begin()).second;
        
       force += *internalPressureConstant * initialNormal * (initialDistance / currentDistance) * initialDistance;
    }

}

inline void Particle::calculateDampingForces() { force -= *c * velocity; }


void Particle::applyForce(float deltaTime) {
    // F = m * a
    // a = F / m
    glm::vec3 acceleration = force / mass;
    // v = v0 + a * t
    velocity += acceleration * deltaTime;
    if(glm::length(velocity)>10.0f)
        velocity = glm::normalize(velocity) * 10.0f;

    // x = x0 + v * t
    position += velocity * deltaTime;

    // reset force for next iteration
    force = glm::vec3(0.0f);
}
