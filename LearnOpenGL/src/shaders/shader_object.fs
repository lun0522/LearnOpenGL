#version 330 core

#define NUM_POINT_LIGHTS 3

in vec3 norm;
in vec3 fragPos;
in vec2 texCoord;
in vec4 fragPosLightSpace;

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
uniform sampler2D depthMap;

float calcShadow(DirLight light, vec3 normal) {
    // this line takes no effect on orthographic projection
    vec3 lightSpaceCoord = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // if further than far plane, this fragment will not be shadowed
    if (lightSpaceCoord.z > 1.0) return 0.0;
    
    lightSpaceCoord = lightSpaceCoord * 0.5 + 0.5; // [-1, 1] -> [0, 1]
    float curDepth = lightSpaceCoord.z;
    // resolution of depth map is limited, so curDepth is precise while hitDepth is not
    // if they are actually equal, but it appears that hitDepth floats around curDepth,
    // which results in stripes. a bias is set to ensure no shadow in this case
    vec3 lightDir = normalize(-light.direction);
    float bias = max(0.002, 0.02 * (1.0 - dot(normal, lightDir)));
    // resolution of depth map is limited, so we retrieve depth of 9 neighbor pixels
    // and compute an average
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for (int i = -1; i < 2; ++i) {
        for (int j = -1; j < 2; ++j) {
            float hitDepth = texture(depthMap, lightSpaceCoord.xy + vec2(i, j) * texelSize).r;
            shadow += curDepth - bias > hitDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, float shadow) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0); // for diffuse
    vec3 halfDir = normalize(lightDir + viewDir); // Blinn-Phong shading
    float spec = pow(max(dot(normal, halfDir), 0.0), material.shininess); // for specular
    
    vec3 ambient  = light.ambient  * texture(material.diffuse0, texCoord).rgb; // unaffected by shadow
    vec3 diffuse  = light.diffuse  * diff * (1.0 - shadow) * texture(material.diffuse0, texCoord).rgb;
    vec3 specular = light.specular * spec * (1.0 - shadow) * texture(material.specular0, texCoord).rgb;
    
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
    float shadow = calcShadow(dirLight, normal);
    
    vec3 outColor = vec3(0.0);
    outColor += calcDirLight(dirLight, normal, viewDir, shadow);
    for (int i = 0; i < NUM_POINT_LIGHTS; ++i)
        outColor += calcPointLight(pointLights[i], normal, viewDir);
    outColor += calcSpotLight(spotLight, normal, viewDir);
    outColor = mix(outColor, calcReflection(normal, viewDir),
                   texture(material.reflection0, texCoord).r);
    
    fragColor = vec4(outColor, 1.0);
}
