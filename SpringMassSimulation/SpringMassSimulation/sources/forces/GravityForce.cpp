#include "forces/GravityForce.h"

void GravityForce::apply(std::vector<Particle*> particles) {
    for (auto p : particles) { p->force += p->mass * gforce; } 
}

void GravityForce::update() {}
GravityForce::~GravityForce() = default;
