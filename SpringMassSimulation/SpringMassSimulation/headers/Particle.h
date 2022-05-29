#pragma once
#include <map>
#include <glm/vec3.hpp>

class Particle {
    std::map<Particle*, float> initialDistancesToNeighbours;
public:
    float mass;
    float k;

    glm::vec3 position = {0, 0, 0};
    glm::vec3 velocity = {0, 0, 0};
    glm::vec3 force = {0, 0, 0};


    Particle(glm::vec3 position, float mass, float springConstant);

    void addNeighbour(Particle* neighbour);

    void applyPhysics(float deltaTime);


private:
    void calculateForce();

    void applyForce(float deltaTime);
};
