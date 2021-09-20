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
uniform sampler2D DuDvMap;
uniform sampler2D normalMap;

uniform PointLight pointLight;
uniform PointLight pointLight2;
uniform Material material;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float moveFactor;
uniform float distortionStrength;
float shineDamper = 20.0;
float reflectivity = 0.6;
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 outColor);
void main() {
    //scaling to (0,1) interval
    vec2 ndcCoord = (clipSpaceCoords.xy/clipSpaceCoords.w) / 2.0 + 0.5;
    vec2 refractionTexCoords = vec2(ndcCoord.x, ndcCoord.y);
    vec2 reflectionTexCoords = vec2(ndcCoord.x, -ndcCoord.y);

    vec2 distortedTexCoords = texture(DuDvMap, vec2(texCoords.x + moveFactor, texCoords.y)).rg*0.1;
    distortedTexCoords = texCoords + vec2(distortedTexCoords.x, distortedTexCoords.y+moveFactor);
    vec2 totalDistortion = (texture(DuDvMap, distortedTexCoords).rg * 2.0 - 1.0) * distortionStrength;
    refractionTexCoords += totalDistortion;
    reflectionTexCoords += totalDistortion;
    //fix distortion around the edge of the screen
    refractionTexCoords =  clamp(refractionTexCoords,0.001,0.999);
    reflectionTexCoords.x = clamp(reflectionTexCoords.x,0.001,0.999);
    reflectionTexCoords.y = clamp(reflectionTexCoords.y,-0.999,-0.001);

    vec4 normalMapColour = texture(normalMap, distortedTexCoords);
    vec3 normal =  vec3(normalMapColour.r * 2.0 - 1.0, normalMapColour.b, normalMapColour.g * 2.0 - 1.0);
    normal = normalize(normal);

    vec4 outColor = mix(texture(reflectionTexture,reflectionTexCoords),texture(refractionTexture,refractionTexCoords),0.5);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 normall = vec3(0.0,1.0,0.0);
    vec3 result = CalcPointLight(pointLight,normal , FragPos,  viewDir, vec3(outColor)) + CalcPointLight(pointLight2,normall , FragPos,  viewDir, vec3(outColor));

    FragColor = vec4(result,1.0);
}
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 outColor)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 125);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * outColor;
    vec3 diffuse = light.diffuse * diff * outColor;
    vec3 specular = light.specular * spec * material.specular;
//     ambient *= attenuation;
//     diffuse *= attenuation;
//     specular *= attenuation;
    return (specular + diffuse + ambient);
}


