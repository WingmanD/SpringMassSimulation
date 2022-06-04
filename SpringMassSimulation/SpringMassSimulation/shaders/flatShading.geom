#version 460 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 150) out;

in vertexData {
    vec3 color;
    vec3 normal;
} vertices[];

out fragmentData {
    vec3 color;
    vec3 normal;
} fragment;

uniform vec3 cameraPos;
uniform mat4 view, projection;

uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float shininess;
uniform float opacity;

uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform float lightIntensity;

void main() {
    vec3 normal = normalize(cross(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz, gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz));

    vec3 triangleCenter = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position).xyz / 3.0;
    vec3 cameraDir = normalize(cameraPos - triangleCenter);

    if (dot(normal, cameraDir) < 0.0) {
        EndPrimitive();
        return;
    }

    for (int i = 0; i < 3; i++) {
        gl_Position = projection * view *  gl_in[i].gl_Position;
        vec3 l = -lightDirection;
        vec3 c = normalize(cameraPos - triangleCenter);
        vec3 r = reflect(l, normal);

        vec3 ambient = ambientColor;
        vec3 diffuse = vec3(0.0);

        float diffuseFactor = dot(l, normal);
        if(diffuseFactor > 0.0) 
            diffuse = diffuseColor * lightColor * lightIntensity * diffuseFactor;
        else diffuse = diffuseColor * lightColor * lightIntensity * diffuseFactor*-0.5;
        

        fragment.color = (ambient + diffuse) * opacity;
        fragment.normal = normal;

        EmitVertex();
    }

    EndPrimitive();
}
