#version 330

uniform mat4 Projection; 
uniform mat4 Model; 
uniform mat4 View; 

layout(location=0) in vec4 Position;
layout(location=1) in vec2 TexCoords;
layout(location=2) in vec3 Normal;

out vec3 position;
out vec3 normal;
out vec2 texCoords;

void main(){
	gl_Position = Projection*View*Model*Position;

	mat4 M = View*Model;
	mat3 NormalMatrix = transpose(inverse(mat3(M)));

	position = vec3(M*Position);
	normal = normalize(NormalMatrix*Normal);
	texCoords = TexCoords;
} 
