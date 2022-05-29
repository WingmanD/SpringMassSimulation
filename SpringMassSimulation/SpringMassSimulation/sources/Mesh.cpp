#include "Mesh.h"

#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Globals.h"

Mesh::Mesh(aiMesh* const meshData) : mesh(meshData) {
    indices.reserve(meshData->mNumFaces * 3);
    vertices.reserve(mesh->mNumVertices);
    
    for (int i = 0; i < mesh->mNumVertices; i++)
        vertices.emplace_back(i, mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
    
    for (int i = 0; i < mesh->mNumFaces; i++) {
        glm::vec3 normal = glm::vec3(0);
        aiVector3D e1, e2;

        e1 = mesh->mVertices[mesh->mFaces[i].mIndices[1]] - mesh->mVertices[mesh->mFaces[i].mIndices[0]];
        e2 = mesh->mVertices[mesh->mFaces[i].mIndices[2]] - mesh->mVertices[mesh->mFaces[i].mIndices[0]];

        Vertex* v0 = &vertices[mesh->mFaces[i].mIndices[0]];
        Vertex* v1 = &vertices[mesh->mFaces[i].mIndices[1]];
        Vertex* v2 = &vertices[mesh->mFaces[i].mIndices[2]];

        v0->addConnected(v1);
        v0->addConnected(v2);
        v1->addConnected(v2);
      
        glm::vec3 edge1 = glm::vec3(e1.x, e1.y, e1.z);
        glm::vec3 edge2 = glm::vec3(e2.x, e2.y, e2.z);

        normal = cross(edge1, edge2);
        normal = normalize(normal);

        vertices[mesh->mFaces[i].mIndices[0]].normal += normal;
        vertices[mesh->mFaces[i].mIndices[1]].normal += normal;
        vertices[mesh->mFaces[i].mIndices[2]].normal += normal;

        for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++) 
            indices.emplace_back(mesh->mFaces[i].mIndices[j]);
    }
    
    for (auto vertex : vertices) vertex.normal = normalize(vertex.normal);
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(3, VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, colour)));

    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));

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

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);

    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, GL_FALSE, &perspective[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE, &model[0][0]);

    glBindVertexArray(0);
}
