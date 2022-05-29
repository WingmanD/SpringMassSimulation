#pragma once

#include "Camera.h"
#include "Scene.h"

class Renderer {
    Camera* camera;
    Scene* scene;
    
public:
    Renderer(Scene* scene, Camera* camera);
    
    void render();

    ~Renderer();
};
