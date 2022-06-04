#include "Renderer.h"
#include "Camera.h"

Renderer::~Renderer() { delete camera; }

Renderer::Renderer(Scene* scene, Camera* camera) {
    this->scene = scene;
    this->camera = camera;
}

void Renderer::render() const {
    for (const auto object : scene->objects) object->drawFirstPass(camera, scene->lights);
    for (const auto object : scene->objects) object->drawSecondPass(camera, scene->lights);
}
