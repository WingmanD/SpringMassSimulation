#include "Mesh.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Globals.h"

Mesh::Mesh(aiMesh* const meshData) : mesh(meshData) {
    vertexNormals = std::vector<glm::vec3>(mesh->mNumVertices);
    indices = std::vector<unsigned int>(mesh->mNumFaces * 3);

    for (int i = 0; i < mesh->mNumFaces; i++) {
        glm::vec3 normal = glm::vec3(0);
        aiVector3D e1, e2;

        e1 = mesh->mVertices[mesh->mFaces[i].mIndices[1]] - mesh->mVertices[mesh->mFaces[i].mIndices[0]];
        e2 = mesh->mVertices[mesh->mFaces[i].mIndices[2]] - mesh->mVertices[mesh->mFaces[i].mIndices[0]];

        glm::vec3 edge1 = glm::vec3(e1.x, e1.y, e1.z);
        glm::vec3 edge2 = glm::vec3(e2.x, e2.y, e2.z);

        normal = glm::cross(edge1, edge2);
        normal = glm::normalize(normal);

        vertexNormals[mesh->mFaces[i].mIndices[0]] += normal;
        vertexNormals[mesh->mFaces[i].mIndices[1]] += normal;
        vertexNormals[mesh->mFaces[i].mIndices[2]] += normal;

        for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
            indices.emplace_back(mesh->mFaces[i].mIndices[j]);
    }

    for (int i = 0; i < vertexNormals.size(); i++)
        vertexNormals[i] = glm::normalize(vertexNormals[i]);

    std::vector<aiColor4D> colors;
    if (!mesh->HasVertexColors(0)) {
        for (int i = 0; i < mesh->mNumVertices; i++) { colors.emplace_back(aiColor4D(0.5f, 0.5f, 0.5f, 1.0f)); }
    }


    glGenVertexArrays(1, &VAO);
    glGenBuffers(3, VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(aiVector3D), mesh->mVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);

    if (!mesh->HasVertexColors(0))
        glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(aiColor4D), &colors.front(), GL_STATIC_DRAW);
    else
        glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(aiColor4D), mesh->mColors[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, vertexNormals.size() * sizeof(glm::vec3), &vertexNormals.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices.front(),
                 GL_STATIC_DRAW);

    glBindVertexArray(0);

}

//TODO translate bounding box to origin and rotate and scale it and translate back
BoundingBox Mesh::getBoundingBox() const {
    glm::vec3 min, max;
    min = max = glm::vec3(mesh->mVertices[0].x, mesh->mVertices[0].y, mesh->mVertices[0].z);

    for (int i = 0; i < mesh->mNumVertices; i++) {
        if (mesh->mVertices[i].x < min.x) { min.x = mesh->mVertices[i].x; }
        if (mesh->mVertices[i].y < min.y) { min.y = mesh->mVertices[i].y; }
        if (mesh->mVertices[i].z < min.z) { min.z = mesh->mVertices[i].z; }

        if (mesh->mVertices[i].x > max.x) { max.x = mesh->mVertices[i].x; }
        if (mesh->mVertices[i].y > max.y) { max.y = mesh->mVertices[i].y; }
        if (mesh->mVertices[i].z > max.z) { max.z = mesh->mVertices[i].z; }
    }

    min = min * Scale;
    max = max * Scale;

    return {min, max};
}

void Mesh::applyTransform(Camera* camera, Transform instanceTransform, Shader* shader) const {
    glm::mat4 perspective = camera->getProjectionMatrix();
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 model = instanceTransform.getModelMatrix();

    glBindVertexArray(VAO);

    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, GL_FALSE, &perspective[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE, &model[0][0]);

    glBindVertexArray(0);
}
