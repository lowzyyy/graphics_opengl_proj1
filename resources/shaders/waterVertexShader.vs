#version 330 core

layout (location = 0) in vec2 vertexPos;

uniform mat4 model;
uniform mat4 pogled;
uniform mat4 projekcija;

vec4 fragWorldPosition;
uniform vec4 plane;
uniform float waterHeight;

out vec2 texCoords;
out vec4 clipSpaceCoords;
out vec3 FragPos;
//usitnjavanje texture
float tiling=6.0;
void main()
{
  fragWorldPosition = model * vec4(vec3(vertexPos.x,waterHeight,vertexPos.y),1.0);
  gl_ClipDistance[0] = dot(fragWorldPosition, plane);
  clipSpaceCoords = projekcija * pogled * fragWorldPosition;
  gl_Position = clipSpaceCoords;
  texCoords = vec2(vertexPos.x/2.0 + 0.5, vertexPos.y/2.0 + 0.5) * tiling;
  FragPos = vec3(fragWorldPosition);
}