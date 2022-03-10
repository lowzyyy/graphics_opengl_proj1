 #version 330 core
out vec4 FragColor;

struct PointLight {
    vec3 position;

    vec3 specular;
    vec3 diffuse;
    vec3 ambient;

    float constant;
    float linear;
    float quadratic;
};

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_normal1;
    sampler2D texture_specular1;
    sampler2D texture_opacity1;
    float shininess;
};
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPosition;
#define MAX_numberOfPointLights 16
uniform int numberOfPointLights;
uniform PointLight pointLights[MAX_numberOfPointLights];
uniform Material material;
// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfWayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfWayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
//     ambient *= attenuation;
//     diffuse *= attenuation;
//     specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - FragPos);
    vec3 result = vec3(0.0);
    float test=0;
    for(int i=0; i<MAX_numberOfPointLights;i++){
        if(i==numberOfPointLights) break;
        result += CalcPointLight(pointLights[i], normal, FragPos, viewDir) ;
    }


    test = float(texture(material.texture_opacity1, TexCoords).r);

    vec4 finalColor = vec4(result,1.0);
    if(test < 0.1)
        discard;
    FragColor = finalColor;
}