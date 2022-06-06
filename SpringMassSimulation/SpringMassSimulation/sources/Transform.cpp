#include "Transform.h"

#include <glm/gtx/transform.hpp>

glm::mat4 Transform::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = translate(model, Location);
    model = glm::rotate(model, Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, Scale);
    return model;
}

glm::mat4 Transform::getViewMatrix() const { return glm::inverse(getModelMatrix()); }
