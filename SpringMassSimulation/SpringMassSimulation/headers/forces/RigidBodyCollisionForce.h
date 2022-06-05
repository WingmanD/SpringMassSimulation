#pragma once
#include "Force.h"

class RigidBodyCollisionForce : public Force {
    glm::vec3 position;
    glm::vec3 direction;
    float D;
public:
    RigidBodyCollisionForce(glm::vec3 position, glm::vec3 direction);
    
    void apply(std::vector<Particle*> particles) override;
    void update() override;
    
    ~RigidBodyCollisionForce() override;
};
