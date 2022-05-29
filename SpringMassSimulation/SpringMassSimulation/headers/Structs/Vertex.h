#pragma once
#include <glad/glad.h>

struct Vertex {
    GLfloat position[3] = {0, 0, 0};
    GLubyte colour[4] = {0, 0, 0, 1};

    Vertex() = default;

    Vertex(const float x, const float y, const float z) {
        position[0] = x;
        position[1] = y;
        position[2] = z;
    }
};
