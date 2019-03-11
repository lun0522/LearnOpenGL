#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 texCoord;

layout (std140) uniform Matrices {
    uniform mat4 view;
    uniform mat4 projection;
};

void main() {
    // ignore translation, so that camera never moves relative to skybox
    gl_Position = projection * mat4(mat3(view)) * vec4(aPos, 1.0);
    gl_Position.zw = vec2(1.0);
    texCoord = aPos;
}
