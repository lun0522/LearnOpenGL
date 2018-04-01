#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT {
    vec3 norm; // in view space
    vec3 fragPos; // in view space
    vec2 texCoord;
} vs_out;

uniform mat3 normal;
uniform mat4 model;
layout (std140) uniform Matrices {
    uniform mat4 view;
    uniform mat4 projection;
};

void main() {
    vs_out.fragPos = vec3(view * model * vec4(aPos, 1.0));
    gl_Position = projection * vec4(vs_out.fragPos, 1.0);
    vs_out.norm = normal * aNormal;
    vs_out.texCoord = aTexCoord;
}
