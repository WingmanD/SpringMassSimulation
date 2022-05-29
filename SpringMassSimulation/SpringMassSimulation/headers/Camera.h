#pragma once
#include "Transform.h"

class Camera : public Transform {
private:
    glm::vec3 center = glm::vec3(0, 0, 0);
    glm::vec3 worldUp = glm::vec3(0, 1, 0);
    glm::vec3 viewUp = glm::vec3(0, 1, 0);
    glm::vec3 front;
    glm::vec3 right;

    float fov = 45.0f;
    float nearPlane = 0.01f;
    float farPlane = 100.0f;
    float focalDistance = 5.0f;

    int width_ = 1280;
    int height_ = 720;

public:
    Camera(glm::vec3 eyeLocation, glm::vec3 center);

    void rotate(glm::vec3 rot);

    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();

    glm::vec3 getViewUp() const;
    glm::vec3 getFront() const;
    glm::vec3 getRight() const;
    
};
