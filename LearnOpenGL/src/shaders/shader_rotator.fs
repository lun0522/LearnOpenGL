#version 330 core

in vec3 norm;
in vec3 fragPos;
in vec2 texCoord;

out vec4 fragColor;

uniform float ambient;
uniform float specular;
uniform float intensity;
uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    // linear interpolation (ratio for first texture and (1 - ratio) for second)
    vec3 texColor = vec3(mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.4));
    vec3 normalDir = normalize(norm);
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float diff = max(dot(normalDir, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0) * specular;
    
    fragColor = vec4(((diff + spec) * lightColor * intensity + ambient) * texColor, 1.0);
}
