#version 330

uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float shininess;

uniform vec4 light_position;
uniform vec3 Ia;
uniform vec3 Id;
uniform vec3 Is;

uniform mat4 View;

in vec3 position;
in vec3 vnormal;

out vec4 FragColor;

void main(){     
	vec3 N = normalize(vnormal);

	// direção do observador
	vec3 wr = normalize(-position); 
	
	// troca a direção da normal caso seja uma backface
	if(!gl_FrontFacing)
		N = -N;

	// Posição da luz no referencial do observador
	vec4 lightPos = View*light_position;

	// Direção da luz
	vec3 wi = (lightPos.w == 0)?
		normalize(lightPos.xyz): // luz direcional
		normalize(lightPos.xyz - position); // luz pontual

	// direção do raio refletido
	vec3 vs = normalize(reflect(-wi, N)); 

	vec3 ambient = Ka*Ia;
	vec3 diffuse = Kd*Id*max(0, dot(wi, N));
	vec3 specular = Ks*Is*pow(max(0,dot(vs, wr)), shininess);

	FragColor = vec4(ambient + diffuse + specular, 1);
}
