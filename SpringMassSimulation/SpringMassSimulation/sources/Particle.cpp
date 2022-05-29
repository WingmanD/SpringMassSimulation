#include "Particle.h"

#include <iostream>
#include <glm/detail/func_geometric.inl>

#include "forces/Force.h"

Particle::Particle(glm::vec3 position, float mass, float springConstant): mass(mass), k(springConstant),
                                                                          position(position) {}

void Particle::addConnected(Particle* connected) {
    initialDistancesToConnected.emplace(connected, distance(position, connected->position));
}

void Particle::addInnerConnected(Particle* innerConnected) {
    initialDistancesToInnerConnected.emplace(innerConnected, distance(position, innerConnected->position));
}

void Particle::applyPhysics(float deltaTime) {
    calculateSpringForces();
    applyForce(deltaTime);
}

void Particle::calculateSpringForces() {
    for (auto initDistConnected : initialDistancesToConnected) {
        float deltaX = initDistConnected.second - distance(position, initDistConnected.first->position);
        // Fs = k * deltaX where k is spring constant and deltaX is the difference between the initial distance and the current distance as vector
        glm::vec3 springForce = k * deltaX * normalize(position - initDistConnected.first->position);
        force += springForce;
    }

    
    for (auto initDistInnerConnected : initialDistancesToInnerConnected) {
        float deltaX = initDistInnerConnected.second - distance(position, initDistInnerConnected.first->position);
        // Fs = k * deltaX where k is spring constant and deltaX is the difference between the initial distance and the current distance as vector
        //todo make this a global constant
        glm::vec3 springForce = 1000.0f * k * deltaX * normalize(position - initDistInnerConnected.first->position);
        force += springForce;
    }
    
}

void Particle::applyForce(float deltaTime) {
    // F = m * a
    // a = F / m
    glm::vec3 acceleration = force / mass;
    // v = v0 + a * t
    velocity += acceleration * deltaTime;

    // x = x0 + v * t
    position += velocity * deltaTime;

    // reset force for next iteration
    force = glm::vec3(0.0f);
}
