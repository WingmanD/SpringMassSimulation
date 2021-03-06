#pragma once

#include <vector>
#include <glm/vec3.hpp>

#include "Mesh.h"

class Particle {
    std::vector<std::pair<Particle*, float>>* initialDistancesToConnected = new std::vector<std::pair<
        Particle*, float>>();
    std::vector<std::pair<Particle*, float>>* initialDistancesToInnerConnected = new std::vector<std::pair<
        Particle*, float>>();
public:
    Mesh* parentMesh;
    float* mass;
    float* k;
    float* k_inner;
    float* c;
    float* internalPressureConstant;

    glm::vec3 normal;
    glm::vec3 color = {0.0f, 1.0f, 0.0f};

    glm::vec3 position = {0, 0, 0};
    glm::vec3 velocity = {0, 0, 0};
    glm::vec3 force = {0, 0, 0};


    Particle(Mesh* parentMesh, glm::vec3 position, glm::vec3 initialNormal, float* mass, float* outerSpringConstant,
             float* innerSpringConstant, float* dampingConstant, float* pressureConstant):
        parentMesh(parentMesh), mass(mass), k(outerSpringConstant), k_inner(innerSpringConstant),
        c(dampingConstant), internalPressureConstant(pressureConstant), normal(initialNormal),
        position(position) { }

    // 
    void addConnected(Particle* connected);
    void addInnerConnected(Particle* innerConnected);

    // physics functions

    
    void applyPhysics(float deltaTime);

    inline void calculateSpringForces();
    inline void calculateInternalPressureForce(float meshSurfaceArea, float meshVolume);
    inline void calculateDampingForces();

    void applyForce(float deltaTime);

};
