#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

vec4 fragWorldPosition;
uniform vec4 plane;
void main()
{
    fragWorldPosition = model * vec4(aPos,1.0);
    gl_ClipDistance[0] = dot(fragWorldPosition, plane);
    FragPos = vec3(fragWorldPosition);
    Normal = mat3(model) * aNormal;
    TexCoords = aTexCoords;    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}