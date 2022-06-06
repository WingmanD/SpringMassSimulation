#pragma once

#include <glm/vec3.hpp>

#include "Shader.h"
#include "assimp/material.h"

class Material {
    std::string name;
    glm::vec3 ambient = {0.1, 0.1, 0.1};
    glm::vec3 diffuse = {0.5, 0.5, 0.5};
    glm::vec3 specular = {1, 1, 1};
    glm::vec3 emissive = {0, 0, 0};
    float shininess = 5;
    float opacity = 1;
public:
    explicit Material(const aiMaterial* material);
    Material() = default;

    void apply(const Shader* shader) const;

    [[nodiscard]] std::string getName() const { return name; }
    [[nodiscard]] glm::vec3 getAmbientColor() const { return ambient; }
    [[nodiscard]] glm::vec3 getDiffuseColor() const { return diffuse; }
    [[nodiscard]] glm::vec3 getSpecularColor() const { return specular; }
    [[nodiscard]] glm::vec3 getEmissiveColor() const { return emissive; }
    [[nodiscard]] float getShininess() const { return shininess; }
    [[nodiscard]] float getOpacity() const { return opacity; }

    void setName(const std::string& newName) { this->name = newName; }
    void setAmbient(const glm::vec3& newAmbient) { this->ambient = newAmbient; }
    void setDiffuse(const glm::vec3& newDiffuse) { this->diffuse = newDiffuse; }
    void setSpecular(const glm::vec3& newSpecular) { this->specular = newSpecular; }
    void setEmissive(const glm::vec3& newEmissive) { this->emissive = newEmissive; }
    void setShininess(float newShininess) { this->shininess = newShininess; }
    void setOpacity(float newOpacity) { this->opacity = newOpacity; }

};
