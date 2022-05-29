#pragma once
#include <vector>

#include "Particle.h"

class Force {
public:
    virtual void apply(std::vector<Particle*> particles) = 0;
    virtual void update() = 0;

    virtual ~Force() = default;
};
