#include "Material.h"

#include <iostream>

#include "Shader.h"
#include <glad/glad.h>

Material::Material(const aiMaterial* material) {
    material->Get(AI_MATKEY_NAME, name);
    material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
    material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
    material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
    material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
    material->Get(AI_MATKEY_SHININESS, shininess);
    material->Get(AI_MATKEY_OPACITY, opacity);
}

void Material::apply(const Shader* shader) const {
    glUniform3fv(glGetUniformLocation(shader->ID, "ambientColor"), 1, &ambient[0]);
    glUniform3fv(glGetUniformLocation(shader->ID, "diffuseColor"), 1, &diffuse[0]);
    glUniform3fv(glGetUniformLocation(shader->ID, "specularColor"), 1, &specular[0]);
    glUniform3fv(glGetUniformLocation(shader->ID, "emissiveColor"), 1, &emissive[0]);

    glUniform1f(glGetUniformLocation(shader->ID, "shininess"), shininess);
    glUniform1f(glGetUniformLocation(shader->ID, "opacity"), opacity);
}
