#pragma once
#include <functional>

#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
#include "Transform.h"
#include "forces/Force.h"

class Object : public Transform {
protected:
    Shader* shader;
public:
    Mesh* mesh;
    std::vector<Force*>* environmentForces{};

    Object(Mesh* mesh, Shader* shader, std::vector<Force*>* environmentForces)
        : mesh(mesh), shader(shader), environmentForces(environmentForces) {}

    std::function<void(double)> tickFunction;
    virtual void tick(double deltaTime) { if(tickFunction) tickFunction(deltaTime); }

    virtual void draw(Camera* camera) { draw(camera, static_cast<Transform>(*this), false); }
    virtual void draw(Camera* camera, Transform instanceTransform, bool selected);

    virtual ~Object() {
        delete mesh;
        delete shader;
    }
};
