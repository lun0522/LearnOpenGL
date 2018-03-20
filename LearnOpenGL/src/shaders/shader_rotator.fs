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
    sampler2D emission;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

uniform Light light;
uniform Material material;
uniform float time;

void main() {
    // emission
    vec3 emission = vec3(texture(material.emission, texCoord)) * (sin(time * 3) + 0.5);
    
    // ambient
    vec3 ambient = vec3(texture(material.diffuse, texCoord)) * light.ambient;
    
    // diffuse
    vec3 normalDir = normalize(norm);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normalDir, lightDir), 0.0);
    vec3 diffuse = diff * vec3(texture(material.diffuse, texCoord)) * light.diffuse;
    
    // specular
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * vec3(texture(material.specular, texCoord)) * light.specular;
    
    fragColor = vec4(emission + ambient + diffuse + specular, 1.0);
}
