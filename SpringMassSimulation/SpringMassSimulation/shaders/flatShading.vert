#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec3 aNormal;

out vertexData {
    vec3 color;
    vec3 normal;
} vertex;

uniform mat4 model;

void main()
{
    vertex.color = aCol;
    vertex.normal = aNormal;
    gl_Position = model * vec4(aPos, 1.0);
}
