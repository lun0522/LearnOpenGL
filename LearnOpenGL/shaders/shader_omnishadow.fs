#version 330 core

in vec4 fragPos;

uniform float frustumHeight;
uniform vec3 lightPos;

void main() {
    float fragLightDist = length(fragPos.xyz - lightPos);
    fragLightDist = fragLightDist / frustumHeight; // true depth -> [0, 1]
    gl_FragDepth = fragLightDist;
}
