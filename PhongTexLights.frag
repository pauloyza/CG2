#version 330

uniform bool has_map_Ka;
uniform sampler2D map_Ka;

uniform bool has_map_Kd;
uniform sampler2D map_Kd;

uniform bool has_map_Ks;
uniform sampler2D map_Ks;

uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float shininess;

#define MAX_LIGHTS 16

uniform struct Light{
	vec4 position;
	vec3 Ia;
	vec3 Id;
	vec3 Is;
}lights[MAX_LIGHTS];
uniform int n_lights;
	
uniform mat4 View;

in vec3 position;
in vec3 normal;
in vec2 texCoords;

out vec4 FragColor;

void main(){     
	vec3 ka = Ka;
	vec3 kd = Kd;
	vec3 ks = Ks;

	float alpha = 1;

	if(has_map_Ka){
		vec4 col = texture(map_Ka, texCoords);
		ka = ka*col.rgb;
	}
	if(has_map_Kd){
		vec4 col = texture(map_Kd, texCoords);
		kd = kd*col.rgb;
		ka = ka*col.rgb;
		alpha = col.a;
	}
	if(has_map_Ks){
		vec4 col = texture(map_Ks, texCoords);
		ks = ks*col.rgb;
	}
	if(alpha < 0.1)
		discard;
	
	// direção do observador
	vec3 wr = normalize(-position); 

	vec3 N = normalize(normal);
	
	// troca a direção da normal caso seja uma backface
	if(!gl_FrontFacing)
		N = -N;

	FragColor = vec4(0, 0, 0, alpha);
	for(int i = 0; i < n_lights; i++){
		// Posição da luz no referencial do observador
		vec4 lightPos = View*lights[i].position;

		// Direção da luz
		vec3 wi = (lightPos.w == 0)?
			normalize(lightPos.xyz): // luz direcional
			normalize(lightPos.xyz - position); // luz pontual

		// direção do raio refletido
		vec3 vs = normalize(reflect(-wi, N)); 

		vec3 ambient = ka*lights[i].Ia;
		vec3 diffuse = kd*lights[i].Id*max(0, dot(wi, N));
		vec3 specular = ks*lights[i].Is*pow(max(0,dot(vs, wr)), shininess);

		FragColor.rgb += ambient + diffuse + specular;
	}
}
