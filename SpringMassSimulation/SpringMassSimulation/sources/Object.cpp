#include "Object.h"

void Object::draw(Camera* camera, Transform instanceTransform, bool selected) {
    glUseProgram(shader->ID);

    mesh->applyTransform(camera, instanceTransform, shader);

    glBindVertexArray(mesh->getVAO());
    glDrawElements(GL_TRIANGLES, mesh->getIndices().size() * sizeof(unsigned int), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
