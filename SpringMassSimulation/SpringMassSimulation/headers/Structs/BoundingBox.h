#pragma once
#include <glm/vec3.hpp>

struct BoundingBox {
    glm::vec3 min, max;

    BoundingBox(const glm::vec3& min, const glm::vec3& max) {
        this->min = min;
        this->max = max;
    }
};
