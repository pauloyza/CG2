#include <GL/glew.h>
#include <GL/freeglut.h>
#include "matrix.h"
#include "Color.h"

unsigned int vao;

const char* source_vs =
	"#version 330\n                 "
	"uniform mat4 M;                "
	"in vec4 v;                     "
	"void main(){                   "
	"    gl_Position = M*v;         "
	"}                              ";

const char* source_fs =
	"#version 330\n                  "
	"uniform vec3 C;                 "
	"out vec4 fragColor;             "
	"void main(){ 	                 "
	"    fragColor = vec4(C, 1);     "
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

	std::vector<vec3> V = {
		{0, 0, 0},
		{1, 0, 0},
		{1, 1, 0},
		{0, 1, 0},
		{0, 0, 1},
		{1, 0, 1},
		{1, 1, 1},
		{0, 1, 1},
	};
	std::vector<unsigned int> indices = {
		0, 1,	1, 2,	2, 3,	3, 0,	4, 5,	5, 6,
		6, 7,	7, 4,	0, 4,	1, 5,	2, 6,	3, 7
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
	glVertexAttribPointer(P_location, 3, GL_FLOAT, GL_FALSE, 0, 0);

	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices.size()*sizeof(int), indices.data(), GL_STATIC_DRAW);

	mat4 Model = rotate_y(0.4)*rotate_x(0.3)*translate(-0.5, -0.5, -0.5);
	mat4 View = translate(0, 0, -2);
	mat4 Projection = frustum(-1, 1, -1, 1, -1, -4);

	mat4 M = Projection*View*Model;
	int loc = glGetUniformLocation(shaderProgram, "M");
	glUniformMatrix4fv(loc, 1, true, &M[0][0]);

	vec3 C = toVec3(green);
	loc = glGetUniformLocation(shaderProgram, "C");
	glUniform3fv(loc, 1, &C.x);

}

void desenha(){
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(vao);
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);

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
