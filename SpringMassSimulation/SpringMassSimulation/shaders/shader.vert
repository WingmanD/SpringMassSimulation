#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec3 normal;

out vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    float cosTheta = clamp(dot(normalize(vec3(model * vec4(normal, 1.0))), normalize(vec3(1))), 0, 1);

    color = aCol * cosTheta;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
