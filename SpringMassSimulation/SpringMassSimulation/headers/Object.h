#pragma once
#include <functional>

#include "Camera.h"
#include "DirectionalLight.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Transform.h"
#include "forces/Force.h"

class Object : public Transform {
protected:
    Shader* shader;
public:
    Mesh* mesh;
    Material* material;
    
    bool bHasCollision = true;
    std::vector<Force*>* environmentForces{};
    

    Object(Mesh* mesh, Material* material, Shader* shader, std::vector<Force*>* environmentForces)
        : shader(shader), mesh(mesh), material(material), environmentForces(environmentForces) {}

    std::function<void(double)> tickFunction;
    virtual void tick(double deltaTime) { if (tickFunction) tickFunction(deltaTime); }

    virtual void drawFirstPass(Camera* camera, std::vector<DirectionalLight*> &lights) { drawFirstPass(camera, static_cast<Transform>(*this),  lights); }
    virtual void drawFirstPass(Camera* camera, Transform instanceTransform, std::vector<DirectionalLight*> &lights);

    virtual void drawSecondPass(Camera* camera, std::vector<DirectionalLight*> &lights) { drawSecondPass(camera, static_cast<Transform>(*this),  lights); }
    virtual void drawSecondPass(Camera* camera, Transform instanceTransform, std::vector<DirectionalLight*> &lights) { }

    virtual ~Object() {
        delete mesh;
        delete shader;
    }
};
