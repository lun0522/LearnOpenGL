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
    sampler2D diffuse0;
    sampler2D specular0;
    sampler2D reflection0;
    samplerCube envMap;
    float shininess;
};

uniform mat3 invView;
uniform DirLight dirLight;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0); // for diffuse
    vec3 halfDir = normalize(lightDir + viewDir); // Blinn-Phong shading
    float spec = pow(max(dot(normal, halfDir), 0.0), material.shininess); // for specular
    
    vec3 ambient  = light.ambient  * texture(material.diffuse0, texCoord).rgb;
    vec3 diffuse  = light.diffuse  * diff * texture(material.diffuse0, texCoord).rgb;
    vec3 specular = light.specular * spec * texture(material.specular0, texCoord).rgb;
    
    return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0); // for diffuse
    vec3 halfDir = normalize(lightDir + viewDir); // Blinn-Phong shading
    float spec = pow(max(dot(normal, halfDir), 0.0), material.shininess); // for specular
    
    // attenuation
    float lightDist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * lightDist + light.quadratic * (lightDist * lightDist));
    
    vec3 ambient  = light.ambient  * texture(material.diffuse0, texCoord).rgb;
    vec3 diffuse  = light.diffuse  * diff * texture(material.diffuse0, texCoord).rgb;
    vec3 specular = light.specular * spec * texture(material.specular0, texCoord).rgb;
    
    return (ambient + diffuse + specular) * attenuation;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0); // for diffuse
    vec3 halfDir = normalize(lightDir + viewDir); // Blinn-Phong shading
    float spec = pow(max(dot(normal, halfDir), 0.0), material.shininess); // for specular
    
    // attenuation
    float lightDist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * lightDist + light.quadratic * (lightDist * lightDist));
    
    // intensity
    float theta = dot(-lightDir, normalize(light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    vec3 ambient  = light.ambient  * texture(material.diffuse0, texCoord).rgb;
    vec3 diffuse  = light.diffuse  * diff * texture(material.diffuse0, texCoord).rgb;
    vec3 specular = light.specular * spec * texture(material.specular0, texCoord).rgb;
    
    return (ambient + (diffuse + specular) * intensity) * attenuation;
}

vec3 calcReflection(vec3 normal, vec3 viewDir) {
    vec3 reflectDir = reflect(-viewDir, normal);
    reflectDir = invView * reflectDir; // back to world coordinate
    return texture(material.envMap, reflectDir).rgb;
}

void main() {
    vec3 normal = normalize(norm);
    vec3 viewDir = normalize(-fragPos);
    
    vec3 outColor = vec3(0.0);
    outColor += calcDirLight(dirLight, normal, viewDir);
    for (int i = 0; i < NUM_POINT_LIGHTS; ++i)
        outColor += calcPointLight(pointLights[i], normal, viewDir);
    outColor += calcSpotLight(spotLight, normal, viewDir);
    outColor = mix(outColor, calcReflection(normal, viewDir),
                   texture(material.reflection0, texCoord).r);
    
    fragColor = vec4(outColor, 1.0);
}
