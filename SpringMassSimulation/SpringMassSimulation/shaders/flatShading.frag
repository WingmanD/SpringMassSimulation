#version 460 core
out vec4 FragColor;

in fragmentData {
    vec3 color;
    vec3 normal;
} fragment;

void main()
{
    FragColor = vec4(fragment.color, 1);  
} 