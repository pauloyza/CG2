#version 330

uniform sampler2D myTexture;

in vec2 texCoords;

out vec4 fragColor;

void main(){
	fragColor = texture(myTexture, texCoords);
}
