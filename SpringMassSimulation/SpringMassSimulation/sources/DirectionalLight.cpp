#include "DirectionalLight.h"

#include <glm/vec3.hpp>

DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec3 color, float intensity) {
    this->direction = direction;
    this->color = color;
    this->intensity = intensity;
}
