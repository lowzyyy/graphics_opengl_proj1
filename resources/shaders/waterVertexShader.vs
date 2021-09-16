#version 330 core

layout (location = 0) in vec2 vertexPos;

uniform mat4 model;
uniform mat4 pogled;
uniform mat4 projekcija;

void main()
{
  gl_Position = projekcija * pogled * model * vec4(vec3(vertexPos.x,0.0,vertexPos.y), 1.0);
}