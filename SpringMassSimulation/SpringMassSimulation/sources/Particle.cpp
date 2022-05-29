#include "Particle.h"

#include <glm/detail/func_geometric.inl>

#include "forces/Force.h"

Particle::Particle(glm::vec3 position, float mass, float springConstant): mass(mass), k(springConstant),
                                                                          position(position) {}

void Particle::addNeighbour(Particle* neighbour) {
    initialDistancesToNeighbours.emplace(neighbour, distance(position, neighbour->position));
}

void Particle::applyPhysics(float deltaTime) {
    calculateForce();
    applyForce(deltaTime);
}

void Particle::calculateForce() {
    for (auto initDistNeighbour : initialDistancesToNeighbours) {
        float deltaX = initDistNeighbour.second - distance(position, initDistNeighbour.first->position);
        // Fs = k * deltaX where k is spring constant and deltaX is the difference between the initial distance and the current distance as vector
        glm::vec3 springForce = k * deltaX * position - initDistNeighbour.first->position;
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
