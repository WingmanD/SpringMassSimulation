#pragma once
#include <vector>

#include "Globals.h"
#include "Object.h"
#include "forces/Force.h"

class Scene {
public:
    std::vector<Object*> objects;
    std::vector<Force*> environmentForces;

    Scene() = default;

    void tick(double deltaTime);

    void load(std::string path, Shader* shader = defaultShader);
    void loadSoft(std::string path, Shader* shader = defaultShader);

    void addObject(Object* object);
    void addForce(Force* force);

    ~Scene();
};
