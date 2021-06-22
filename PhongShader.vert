#version 330

uniform mat4 Projection; 
uniform mat4 Model; 
uniform mat4 View; 

layout(location=0) in vec4 Position;
layout(location=2) in vec3 Normal;

out vec3 position;
out vec3 vnormal;

void main(){
	gl_Position = Projection*View*Model*Position;

	mat4 M = View*Model;
	mat3 NormalMatrix = transpose(inverse(mat3(M)));

	position = vec3(M*Position);
	vnormal = normalize(NormalMatrix*Normal);
} 
