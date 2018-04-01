#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// although vertex shader only outputs attributes of a single vertex,
// geometry shader receives attrbutes of several vertices (depends on
// input type), and thus gs_in is an array
in VS_OUT {
    vec3 norm;
    vec3 fragPos;
    vec2 texCoord;
} gs_in[];

// fragment shader only wants attributes of a single vertex
// current value of these output vectors will be sent along with that
// vertex when EmitVertex() is called
out vec3 norm;
out vec3 fragPos;
out vec2 texCoord;

uniform float explosion;
layout (std140) uniform Matrices {
    uniform mat4 view;
    uniform mat4 projection;
};

vec3 getNormal() {
    vec3 a = gs_in[0].fragPos.xyz - gs_in[2].fragPos.xyz;
    vec3 b = gs_in[1].fragPos.xyz - gs_in[2].fragPos.xyz;
    return normalize(cross(a, b));
}

void explode(int index, vec3 movement) {
    norm = gs_in[index].norm;
    fragPos = gs_in[index].fragPos + movement;
    texCoord = gs_in[index].texCoord;
    gl_Position = projection * vec4(fragPos, 1.0);
    EmitVertex();
}

void main() {
    vec3 movement = getNormal() * explosion;
    explode(0, movement);
    explode(1, movement);
    explode(2, movement);
    EndPrimitive();
}
