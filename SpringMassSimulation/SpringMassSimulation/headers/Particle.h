#pragma once
#include <map>
#include <glm/vec3.hpp>

class Particle {
    std::map<Particle*, float> initialDistancesToConnected;
    std::map<Particle*, float> initialDistancesToInnerConnected;
    glm::vec3 initialNormal;
public:
    float mass;
    float* k;
    float* k_inner;
    float* c;
    float* internalPressureConstant;

    glm::vec3 position = {0, 0, 0};
    glm::vec3 velocity = {0, 0, 0};
    glm::vec3 force = {0, 0, 0};


    Particle(glm::vec3 position, glm::vec3 initialNormal, float mass, float* outerSpringConstant,
             float* innerSpringConstant, float* dampingConstant, float* pressureConstant):
        initialNormal(initialNormal), mass(mass), k(outerSpringConstant), k_inner(innerSpringConstant),
        c(dampingConstant), internalPressureConstant(pressureConstant), position(position) {}

    void addConnected(Particle* connected);
    void addInnerConnected(Particle* innerConnected);
    
    void applyPhysics(float deltaTime);

    inline void calculateSpringForces();
    inline void calculateInternalPressureForce();
    inline void calculateDampingForces();

    void applyForce(float deltaTime);
    
};
