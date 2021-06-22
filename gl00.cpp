#include <GL/glew.h>
#include <GL/freeglut.h>
#include "matrix.h"
#include "Color.h"

unsigned int vao;

const char* source_vs =
	"#version 330\n                 "
	"in vec4 v;                     "
	"void main(){                   "
	"    gl_Position = v;           "
	"}                              ";

const char* source_fs =
	"#version 330\n                  "
	"out vec4 fragColor;             "
	"void main(){ 	                 "
	"    fragColor = vec4(0,0,1,1);  "
	"}                               ";


void init(){
	glewInit();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &source_vs, NULL);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &source_fs, NULL);
	glCompileShader(fragmentShader);

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	std::vector<vec2> V = {
		{-0.7, -0.5},
		{-0.8, 0.2},
		{0.0, -0.35},
		{-0.37, 0.25},
		{0.64, 0.58},
		{0.45, -0.23},
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned int vbo_position;
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER,
		V.size()*sizeof(vec3), V.data(), GL_STATIC_DRAW);

	int P_location = glGetAttribLocation(shaderProgram, "v");
	glEnableVertexAttribArray(P_location);
	glVertexAttribPointer(P_location, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void desenha(){
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(vao);
	glDrawArrays(GL_LINE_LOOP, 0, 6);

	glutSwapBuffers();
}

int main(int argc, char* argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	glutInitWindowSize(500, 500);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("janela");
	glutDisplayFunc(desenha);

	printf("GL Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	init();
	glutMainLoop();
}
