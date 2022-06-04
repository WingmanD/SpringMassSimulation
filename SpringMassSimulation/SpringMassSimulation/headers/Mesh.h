#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Camera.h"
#include "Shader.h"
#include "assimp/mesh.h"
#include "Structs/BoundingBox.h"

struct Vertex {
    int index = -1;
    glm::vec3 position = {0, 0, 0};
    glm::vec3 colour = {0.5f, 0.5f, 0.5f};
    glm::vec3 normal = {0, 0, 0};
    glm::vec2 uv = {0, 0};

    std::vector<Vertex*> connected;

    Vertex() = default;

    Vertex(int index, const float x, const float y, const float z) {
        this->index = index;
        position.x = x;
        position.y = y;
        position.z = z;
    }

    Vertex(int index, glm::vec3 pos) {
        this->index = index;
        position = pos;
    }

    void addConnected(Vertex* v) {
        if (std::ranges::find(connected, v) == connected.end()) {
            connected.push_back(v);
            v->addConnected(this);
        }
    }
};

struct Triangle {
    Vertex* v0;
    Vertex* v1;
    Vertex* v2;

    Triangle(Vertex* v0, Vertex* v1, Vertex* v2) {
        this->v0 = v0;
        this->v1 = v1;
        this->v2 = v2;
    }

    [[nodiscard]] glm::vec3 getNormal() const { return normalize(v0->normal + v1->normal + v2->normal); }
    [[nodiscard]] glm::vec3 getCenter() const { return (v0->position + v1->position + v2->position) / 3.0f; }

    [[nodiscard]] glm::vec3 getPhysicalNormal() const {
        return normalize(cross(v1->position - v0->position, v2->position - v0->position));
    }

    [[nodiscard]] float getArea() const {
        glm::vec3 e0 = v1->position - v0->position;
        glm::vec3 e1 = v2->position - v0->position;

        return length(cross(e0, e1)) / 2.0f;
    }
};


class Mesh : public Transform {
    GLuint VAO{};
    GLuint VBO[3]{};
    GLuint EBO{};
public:
    std::vector<unsigned int> indices;
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;

    Mesh(aiMesh* meshData);

    [[nodiscard]] BoundingBox getModelSpaceBoundingBox() const;

    void applyTransform(Camera* camera, Transform instanceTransform, Shader* shader) const;

    [[nodiscard]] std::vector<unsigned> getIndices() const { return indices; }
    [[nodiscard]] GLuint getVAO() const { return VAO; }
    [[nodiscard]] GLuint getEBO() const { return EBO; }
    [[nodiscard]] std::vector<glm::vec3> calculateNormals() const;

    ~Mesh() {
        glDeleteBuffers(3, VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }
};
