#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/vec3.hpp>

#include "Camera.h"
#include "Shader.h"
#include "assimp/mesh.h"
#include "Structs/BoundingBox.h"

class Mesh : public Transform {
private:
    aiMesh* mesh;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> vertexNormals;

    GLuint VAO;
    GLuint VBO[3];
    GLuint EBO;
public:
    Mesh(aiMesh* meshData);

    BoundingBox getBoundingBox() const;

    void applyTransform(Camera* camera, Transform instanceTransform, Shader* shader) const;

    aiMesh* getMesh() const { return mesh; }
    std::vector<unsigned> getIndices() const { return indices; }
    std::vector<glm::vec3> getVertexNormals() const { return vertexNormals; }
    GLuint getVAO() const { return VAO; }
    GLuint getEBO() const { return EBO; }

    ~Mesh() {
        glDeleteBuffers(3, VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }
};
