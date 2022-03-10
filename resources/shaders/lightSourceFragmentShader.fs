#version 330 core

in vec3 inNormal;

uniform vec3 lightSourceColor;

out vec4 FragColor;

void main() {
    FragColor = vec4(lightSourceColor,1.0);
}