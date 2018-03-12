#version 330 core

in vec2 textureCoord;

out vec4 fragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    // linear interpolation (ratio for first texture and (1 - ratio) for second)
    fragColor = mix(texture(texture1, textureCoord), texture(texture2, textureCoord), 0.4);
}
