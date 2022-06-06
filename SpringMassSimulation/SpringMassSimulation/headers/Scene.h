#pragma once

#include <vector>

#include "Globals.h"
#include "Object.h"
#include "forces/Force.h"

class Scene {
public:
    std::vector<Object*> objects;
    std::vector<DirectionalLight*> lights;
    std::vector<Force*> environmentForces;
    Camera* activeCamera;

    Scene(Camera* camera) : activeCamera(camera) {}

    void tick(double deltaTime);

    Object* load(std::string path, Shader* shader = defaultShader, bool bLoadMaterials = true);
    Object* loadSoft(std::string path, Shader* shader = defaultShader, bool bLoadMaterials = true);

    void addObject(Object* object);
    void addLight(DirectionalLight* light);
    void addForce(Force* force);

    ~Scene();
};
