#version 330 core
struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct DirectionalLight {
     vec3 direction;

     vec3 specular;
     vec3 diffuse;
     vec3 ambient;
};
struct Material {

    vec3 specular;
    float shininess;
};
in vec4 clipSpaceCoords;
in vec2 texCoords;
in vec3 FragPos;
out vec4 FragColor;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform samplerCube skybox;
uniform sampler2D DuDvMap;
uniform sampler2D normalMap;

uniform Material material;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool specularWater;
uniform float mixRatio;

uniform float moveFactor;
uniform float distortionStrength;
float shineDamper = 20.0;
float reflectivity = 0.6;

#define MAX_numberOfPointLights 16
uniform int numberOfPointLights;
uniform PointLight pointLights[MAX_numberOfPointLights];
uniform DirectionalLight dirLight;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 outColor);
vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir,vec3 outColor);
vec3 CalcPointLight_nospec(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 outColor);
vec3 CalcDirLight_nospec(DirectionalLight light, vec3 normal, vec3 viewDir,vec3 outColor);
void main() {
    //scaling to (0,1) interval to sample the texture
    vec2 ndcCoord = (clipSpaceCoords.xy/clipSpaceCoords.w) / 2.0 + 0.5;
    vec2 refractionTexCoords = vec2(ndcCoord.x, ndcCoord.y);
    vec2 reflectionTexCoords = vec2(ndcCoord.x, -ndcCoord.y);

    vec2 distortedTexCoords = texture(DuDvMap, vec2(texCoords.x + moveFactor, texCoords.y)).rg*0.1;
    distortedTexCoords = texCoords + vec2(distortedTexCoords.x, distortedTexCoords.y+moveFactor);
    vec2 totalDistortion = (texture(DuDvMap, distortedTexCoords).rg * 2.0 - 1.0) * distortionStrength;
    refractionTexCoords += totalDistortion;
    reflectionTexCoords += totalDistortion;
    //fix distortion around the edge of the screen
    refractionTexCoords =  clamp(refractionTexCoords,0.0001,0.9999);
    reflectionTexCoords.x = clamp(reflectionTexCoords.x,0.0001,0.9999);
    reflectionTexCoords.y = clamp(reflectionTexCoords.y,-0.9999,-0.0001);

    vec4 normalMapColour = texture(normalMap, distortedTexCoords);
    vec3 normal =  vec3(normalMapColour.r * 2.0 - 1.0, normalMapColour.b, normalMapColour.g * 2.0 - 1.0);
    normal = normalize(normal);

    vec4 outColor = mix(texture(reflectionTexture,reflectionTexCoords),texture(refractionTexture,refractionTexCoords),0.5);

    vec3 viewDir = normalize(viewPos - FragPos);

    //mix with skybox
    vec3 viewDirection = normalize(FragPos - viewPos);
    vec3 reflectionVector = reflect(viewDirection, vec3(0.0,1.0,0.0));
    reflectionVector.xy += totalDistortion;
    outColor = mix(outColor,texture(skybox,reflectionVector),mixRatio);
//     outColor =  vec4(texture(skybox,reflectionVector).rgb,1.0);
    if(specularWater){
        vec3 result = CalcDirLight(dirLight,normal,viewDir,vec3(outColor));
        for(int i=0; i<MAX_numberOfPointLights;i++){
                if(i==numberOfPointLights) break;
                result += CalcPointLight(pointLights[i], normal, FragPos, viewDir,vec3(outColor));
            }
        FragColor = vec4(result,1.0);
    }
    else{
        vec3 result = CalcDirLight_nospec(dirLight,normal,viewDir,vec3(outColor));
        for(int i=0; i<MAX_numberOfPointLights;i++){
                        if(i==numberOfPointLights) break;
                        result += CalcPointLight_nospec(pointLights[i], normal, FragPos, viewDir,vec3(outColor));
                    }
        FragColor = vec4(result,1.0);
//         FragColor = outColor;
    }

}
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 outColor)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.45);
    // specular shading
//     vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfWayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfWayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * outColor;
    vec3 diffuse = light.diffuse * diff * outColor;
    vec3 specular = light.specular * spec * material.specular;
//     ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (specular + diffuse + ambient);
}
vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir,vec3 outColor)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfWayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfWayDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * outColor;
    vec3 diffuse = light.diffuse * diff * outColor;
    vec3 specular = light.specular * spec * material.specular;
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight_nospec(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 outColor)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.45);
    // specular shading
//     vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfWayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfWayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * outColor;
    vec3 diffuse = light.diffuse * diff * outColor;
    vec3 specular = light.specular * spec * material.specular;
//     ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return ( diffuse + ambient);
}
vec3 CalcDirLight_nospec(DirectionalLight light, vec3 normal, vec3 viewDir,vec3 outColor)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfWayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfWayDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * outColor;
    vec3 diffuse = light.diffuse * diff * outColor;
    vec3 specular = light.specular * spec * material.specular;
    return (ambient + diffuse );
}


