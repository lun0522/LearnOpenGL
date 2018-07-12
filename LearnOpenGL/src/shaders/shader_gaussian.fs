#version 330 core

in vec2 texCoord;

out vec4 fragColor;

uniform bool horizontal;
uniform sampler2D texture1;

const float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {
    vec2 pixelSize = 1.0 / textureSize(texture1, 0);
    vec3 color = texture(texture1, texCoord).rgb * weight[0];
    if (horizontal) {
        for (int i = 1; i < 5; ++i) {
            color += texture(texture1, texCoord + vec2(pixelSize.x * i, 0.0)).rgb * weight[i];
            color += texture(texture1, texCoord - vec2(pixelSize.x * i, 0.0)).rgb * weight[i];
        }
    } else {
        for (int i = 1; i < 5; ++i) {
            color += texture(texture1, texCoord + vec2(0.0, pixelSize.y * i)).rgb * weight[i];
            color += texture(texture1, texCoord - vec2(0.0, pixelSize.y * i)).rgb * weight[i];
        }
    }
    fragColor = vec4(color, 1.0);
}
