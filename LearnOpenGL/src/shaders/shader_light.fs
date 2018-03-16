#version 330 core

out vec4 fragColor;

uniform float lightIntensity;
uniform vec3 lightColor;

void main() {
    fragColor = vec4(lightColor, 1.0) * lightIntensity;
}
