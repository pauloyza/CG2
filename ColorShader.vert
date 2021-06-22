#version 330

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

layout (location = 0) in vec4 in_position;
layout (location = 1) in vec3 in_color;

out vec3 v_color;

void main(){
	gl_Position = Projection*View*Model*in_position;
	v_color = in_color;
}
