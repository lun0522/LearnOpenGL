#version 330 core

in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D texture1;

void main() {
    vec3 color = texture(texture1, texCoord).rgb;
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) fragColor = vec4(color, 1.0);
    else fragColor = vec4(0.0);
}
