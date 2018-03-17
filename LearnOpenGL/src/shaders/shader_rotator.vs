#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 norm; // in view space
out vec3 fragPos; // in view space
out vec2 texCoord;

uniform mat4 model;
uniform mat3 normal;
uniform mat4 view;
uniform mat4 projection;

void main() {
    fragPos = vec3(view * model * vec4(aPos, 1.0));
    gl_Position = projection * vec4(fragPos, 1.0);
    norm = normal * aNormal;
    texCoord = aTexCoord;
}
