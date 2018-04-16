#version 330 core

in vec2 texCoord;

out vec4 fragColor;

uniform vec3 color;
uniform sampler2D text;

void main() {
    fragColor = vec4(color, texture(text, texCoord).r);
}
