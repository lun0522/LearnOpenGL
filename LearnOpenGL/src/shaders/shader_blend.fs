#version 330 core

in vec2 texCoord;

out vec4 fragColor;

uniform float exposure;
uniform sampler2D scene;
uniform sampler2D bloom;

void main() {
    vec3 hdrColor = texture(scene, texCoord).rgb;
    vec3 bloomColor = texture(bloom, texCoord).rgb;
    fragColor = vec4(vec3(1.0) - exp(-(hdrColor + bloomColor) * exposure), 1.0);
}
