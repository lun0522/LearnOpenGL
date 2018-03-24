#version 330 core

#define NUM_POINT_LIGHTS 3

in vec3 norm;
in vec3 fragPos;
in vec2 texCoord;

out vec4 fragColor;

struct DirLight {
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float innerCutOff;
    float outerCutOff;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};

struct Material {
    sampler2D emission;
    sampler2D diffuse0;
    sampler2D specular0;
    float shininess;
};

uniform DirLight dirLight;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform float emitIntensity;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0); // for diffuse
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse0, texCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse0, texCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular0, texCoord));
    
    return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0); // for diffuse
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); // for specular
    
    // attenuation
    float lightDist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * lightDist + light.quadratic * (lightDist * lightDist));
    
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse0, texCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse0, texCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular0, texCoord));
    
    return (ambient + diffuse + specular) * attenuation;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0); // for diffuse
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); // for specular
    
    // attenuation
    float lightDist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * lightDist + light.quadratic * (lightDist * lightDist));
    
    // intensity
    float theta = dot(-lightDir, normalize(light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse0, texCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse0, texCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular0, texCoord));
    
    return (ambient + (diffuse + specular) * intensity) * attenuation;
}

void main() {
    vec3 normal = normalize(norm);
    vec3 viewDir = normalize(-fragPos);
    
    vec3 outColor = vec3(0.0);
    outColor += calcDirLight(dirLight, normal, viewDir);
    for (int i = 0; i < NUM_POINT_LIGHTS; ++i)
        outColor += calcPointLight(pointLights[i], normal, viewDir);
    outColor += calcSpotLight(spotLight, normal, viewDir);
    
    fragColor = vec4(outColor, 1.0);
}
