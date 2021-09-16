#version 330 core

in vec3 inColor;
out vec4 FragColor;


uniform sampler2D t0;
uniform sampler2D t1;

uniform float p;

void main()
{
  FragColor = vec4(inColor,0.7);
}