#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in mat4 instanceModel;

out vec2 texCoord;

layout (std140) uniform Matrices {
    uniform mat4 view;
    uniform mat4 projection;
};

void main() {
    gl_Position = projection * view * instanceModel * vec4(aPos, 1.0);
    texCoord = aTexCoord;
}