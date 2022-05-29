#include "Camera.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Globals.h"

Camera::Camera(glm::vec3 eyeLocation, glm::vec3 center) {
    Location = eyeLocation;
    this->center = center;
    this->width_ = width;
    this->height_ = height;

    front = center - eyeLocation;
    focalDistance = glm::length(front);

    front = glm::normalize(front);
    right = glm::normalize(glm::cross(front, worldUp));
    viewUp = glm::normalize(glm::cross(right, front));
}

void Camera::rotate(glm::vec3 rot) {
    glm::mat4 rM = glm::mat4(1.0f);
    rM = glm::rotate(rM, -rot.z, glm::vec3(0, 1, 0));
    front = glm::vec3(rM * glm::vec4(front, 1.0f));
    right = glm::vec3(rM * glm::vec4(right, 1.0f));
    viewUp = glm::vec3(rM * glm::vec4(viewUp, 1.0f));

    rM = glm::mat4(1.0f);
    rM = glm::rotate(rM, rot.y, right);
    glm::vec3 newFront = glm::vec3(rM * glm::vec4(front, 1.0f));
    glm::vec3 newViewUp = glm::vec3(rM * glm::vec4(viewUp, 1.0f));

    float angle = glm::degrees(glm::angle(newFront, glm::vec3(newFront.x, 0, newFront.z)));
    if (angle < 89.0f) {
        front = newFront;
        viewUp = newViewUp;
    }
}

glm::mat4 Camera::getViewMatrix() {
    right = glm::normalize(glm::cross(front, viewUp));
    viewUp = glm::normalize(glm::cross(right, front));
    glm::mat4 view = glm::lookAt(Location, Location + front, viewUp);

    return view;
}

glm::mat4 Camera::getProjectionMatrix() {
    float aspectRatio = static_cast<float>(width_) / static_cast<float>(height_);

    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

glm::vec3 Camera::getViewUp() const { return viewUp; }
glm::vec3 Camera::getFront() const { return front; }
glm::vec3 Camera::getRight() const { return right; }
