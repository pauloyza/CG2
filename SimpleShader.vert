#version 330

uniform mat4 M;
layout (location=0) in vec4 v;

void main(){
    gl_Position = M*v;
}


