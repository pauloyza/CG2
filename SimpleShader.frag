#version 330

uniform vec3 C;
out vec4 fragColor;

void main(){
    fragColor = vec4(C, 1);
}
