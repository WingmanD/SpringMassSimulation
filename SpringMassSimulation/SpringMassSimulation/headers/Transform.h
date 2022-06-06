#pragma once

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

class Transform {
public:
    glm::vec3 Location = glm::vec3(0), Rotation = glm::vec3(0), Scale = glm::vec3(1);

    void setLocation(glm::vec3 newLocation) { Location = newLocation; }
    void setRotation(glm::vec3 newRotation) { Rotation = newRotation; }
    void setScale(glm::vec3 newScale) { Scale = newScale; }

    void move(glm::vec3 direction) { Location += direction; }
    void rotate(glm::vec3 eulerAngles) { Rotation += eulerAngles; }
    void scale(glm::vec3 factor) { Scale *= factor; }

    [[nodiscard]] glm::mat4 getModelMatrix() const;
    [[nodiscard]] glm::mat4 getViewMatrix() const;
};
