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
    int index;
    glm::vec3 position = {0, 0, 0};
    glm::vec4 colour = {0.5f, 0.5f, 0.5f, 1};
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
    Vertex* v1;
    Vertex* v2;
    Vertex* v3;

    Triangle(Vertex* v1, Vertex* v2, Vertex* v3) {
        this->v1 = v1;
        this->v2 = v2;
        this->v3 = v3;
    }

    glm::vec3 getNormal() const { return normalize(cross(v2->position - v1->position, v3->position - v1->position)); }
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

    [[nodiscard]] BoundingBox getBoundingBox() const;

    void applyTransform(Camera* camera, Transform instanceTransform, Shader* shader) const;

    [[nodiscard]] std::vector<unsigned> getIndices() const { return indices; }
    [[nodiscard]] GLuint getVAO() const { return VAO; }
    [[nodiscard]] GLuint getEBO() const { return EBO; }

    ~Mesh() {
        glDeleteBuffers(3, VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }
};
