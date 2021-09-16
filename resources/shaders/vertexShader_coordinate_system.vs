#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 inColor;

uniform mat4 model;
uniform mat4 pogled;
uniform mat4 projekcija;
void main()
{
  gl_Position =   projekcija * pogled * model * vec4(position,1.0);
  inColor = color;

}