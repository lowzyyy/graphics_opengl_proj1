#version 330 core
out vec4 FragColor;

in vec2 inVertexTexCoord;

uniform sampler2D map;

void main(){
    FragColor = texture(map,inVertexTexCoord);
}