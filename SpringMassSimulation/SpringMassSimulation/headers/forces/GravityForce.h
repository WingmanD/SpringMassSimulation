#pragma once
#include "forces/Force.h"

class GravityForce : public Force {
public:
    glm::vec3* gforce;

    explicit GravityForce(glm::vec3* gforce)
        : gforce(gforce) {}

    void apply(std::vector<Particle*> particles) override;
    void update() override;

    ~GravityForce() override;
};
