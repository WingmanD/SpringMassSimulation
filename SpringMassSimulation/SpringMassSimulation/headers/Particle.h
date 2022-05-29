#pragma once
#include <map>
#include <glm/vec3.hpp>

class Particle {
    std::map<Particle*, float> initialDistancesToConnected;
    std::map<Particle*, float> initialDistancesToInnerConnected;
public:
    float mass;
    float k;

    glm::vec3 position = {0, 0, 0};
    glm::vec3 velocity = {0, 0, 0};
    glm::vec3 force = {0, 0, 0};


    Particle(glm::vec3 position, float mass, float springConstant);

    void addConnected(Particle* connected);
    void addInnerConnected(Particle* innerConnected);

    void applyPhysics(float deltaTime);

    void calculateSpringForces();

    void applyForce(float deltaTime);
    
};
