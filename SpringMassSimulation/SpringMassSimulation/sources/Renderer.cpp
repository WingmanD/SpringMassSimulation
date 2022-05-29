#include "Renderer.h"
#include "Camera.h"

Renderer::~Renderer() { delete camera; }

Renderer::Renderer(Scene* scene, Camera* camera) {
    this->scene = scene;
    this->camera = camera;
}

void Renderer::render() { for (auto object : scene->objects) object->draw(camera); }
