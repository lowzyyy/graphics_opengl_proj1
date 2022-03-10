#version 330 core
layout (location = 0) in vec2 vertexPos;
layout (location = 1) in vec2 vertexTexCoord;

uniform mat4 model;
out vec2 inVertexTexCoord;

void main(){
    gl_Position = model * vec4(vertexPos,0.0,1.0);
    inVertexTexCoord = vertexTexCoord ;
}