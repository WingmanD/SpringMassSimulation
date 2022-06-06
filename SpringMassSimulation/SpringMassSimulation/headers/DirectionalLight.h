#pragma once

#include "Transform.h"

class DirectionalLight : public Transform {
    glm::vec3 color{1, 1, 1};
    glm::vec3 direction = glm::vec3(0, -1, 0);
    float intensity = 1.0f;
public:
    DirectionalLight(glm::vec3 direction, glm::vec3 color, float intensity);

    glm::vec3 getColor() const { return color; }
    glm::vec3 getDirection() const { return direction; }
    float getIntensity() const { return intensity; }

    void setColor(const glm::vec3& newColor) { this->color = newColor; }
    void setDirection(const glm::vec3& newDirection) { this->direction = newDirection; }
    void setIntensity(float newIntensity) { this->intensity = newIntensity; }
};
