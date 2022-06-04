#include "Object.h"

void Object::drawFirstPass(Camera* camera, Transform instanceTransform, std::vector<DirectionalLight*>& lights) {
    glUseProgram(shader->ID);

    mesh->applyTransform(camera, instanceTransform, shader);

    if (!lights.empty()) {
        glUniform3fv(glGetUniformLocation(shader->ID, "lightDirection"), 1, &lights[0]->getDirection()[0]);
        glUniform3fv(glGetUniformLocation(shader->ID, "lightColor"), 1, &lights[0]->getColor()[0]);
        glUniform1f(glGetUniformLocation(shader->ID, "lightIntensity"), lights[0]->getIntensity());
    }

    material->apply(shader);

    glBindVertexArray(mesh->getVAO());
    glDrawElements(GL_TRIANGLES, mesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
