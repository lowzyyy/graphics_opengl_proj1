#version 330 core

layout (location = 0) in vec3 vertexPos;

uniform mat4 model;
uniform mat4 pogled;
uniform mat4 projekcija;

void main()
{
  gl_Position = projekcija * pogled * model * vec4(vertexPos, 1.0);
}