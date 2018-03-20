#version 330 core

in vec3 norm;
in vec3 fragPos;
in vec2 texCoord;

out vec4 fragColor;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Light light;
uniform Material material;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    vec3 ambient = material.ambient * light.ambient;
    
    vec3 normalDir = normalize(norm);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normalDir, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse * light.diffuse;
    
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular * light.specular;
    
    // linear interpolation (ratio for first texture and (1 - ratio) for second)
    vec3 texColor = vec3(mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.4));
    fragColor = vec4((ambient + diffuse + specular) * texColor, 1.0);
}
