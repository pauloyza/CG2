#version 330

uniform mat4 M;

layout (location = 0) in vec4 in_position;
layout (location = 1) in vec2 in_texCoords;

out vec2 texCoords;

void main(){
	gl_Position = M*in_position;
	
	texCoords = in_texCoords;
}
