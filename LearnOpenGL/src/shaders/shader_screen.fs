#version 330 core

in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D texture1;

void main() {
    vec3 linearColor = texture(texture1, texCoord).rgb;
    // gamma correction at the very last step
    fragColor = vec4(pow(linearColor, vec3(1.0 / 2.2)), 1.0);
}
