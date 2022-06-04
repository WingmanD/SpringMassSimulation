#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;

out vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    color = aCol;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    gl_PointSize = 10.0;
}
