#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

out vec4 fragPos;

uniform mat4 lightSpace[6];

void main() {
    for (int face = 0; face < 6; ++face) {
        gl_Layer = face; // control which face of cubemap to emit primitive to
        for (int i = 0; i < 3; ++i) {
            fragPos = gl_in[i].gl_Position;
            gl_Position = lightSpace[face] * fragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}
