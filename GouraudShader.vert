#version 330

uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float shininess;

uniform vec4 light_position;
uniform vec3 Ia;
uniform vec3 Id;
uniform vec3 Is;

uniform mat4 Projection; 
uniform mat4 Model; 
uniform mat4 View; 

layout(location=0) in vec4 Position;
layout(location=2) in vec3 Normal;

out vec3 v_color;

void main(){
	gl_Position = Projection*View*Model*Position;

	mat4 M = View*Model;
	mat3 NormalMatrix = transpose(inverse(mat3(M)));

	vec3 position = vec3(M*Position);
	vec3 N = normalize(NormalMatrix*Normal);
	
	// direção do observador
	vec3 wr = normalize(-position); 

	// Posição da luz no referencial do observador
	vec4 lightPos = View*light_position;
	
	// Direção da luz
	vec3 wi = (lightPos.w == 0)?
		normalize(lightPos.xyz): // luz direcional
		normalize(lightPos.xyz - position); // luz pontual


	// direção do raio refletido
	vec3 vs = normalize(reflect(-wi, N)); 

	vec3 ambient  = Ka*Ia;
	vec3 diffuse  = Kd*Id*max(0, dot(wi, N));
	vec3 specular = Ks*Is*pow(max(0,dot(vs, wr)), shininess);

	v_color = ambient + diffuse + specular;
} 
