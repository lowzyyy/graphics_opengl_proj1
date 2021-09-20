#version 330 core

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 color;

out vec3 inColor;

uniform mat4 model;
uniform mat4 pogled;
uniform mat4 projekcija;

vec4 fragWorldPosition;
uniform vec4 plane;
void main()
{
  fragWorldPosition = model * vec4(vertexPos,1.0);
  gl_ClipDistance[0] = dot(fragWorldPosition, plane);
  gl_Position = projekcija * pogled * fragWorldPosition;
  inColor = color;

}