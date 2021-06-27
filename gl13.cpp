#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Color.h"
#include "GLutils.h"
#include "MarchingCubes.h"

VAO vao;
GLBuffer vbo_position, vbo_normal;
int n_verts;
float angle = 0;
	
float treco(float x, float y, float z){
	float r = (x*x + y*y);
	return 2*y*(y*y - 3*x*x)*(1-z*z) + r*r - (9*z*z - 1)*(1 - z*z);
}

vec3 normal_treco(float x, float y, float z){
	return normalize({
		4*(x*x*x + x*y*(y + 3*z*z - 3)),
		x*x*(4*y + 6*z*z -6) + 2*y*y*(2*y - 3*z*z +3),
		4*z*(3*x*x*y - y*y*y + 9*z*z - 5)
	});
}

void init_buffers(const std::vector<vec3>& V, const std::vector<vec3>& Normal){
	vao = VAO{true};
	glBindVertexArray(vao);

	vbo_position = GLBuffer{GL_ARRAY_BUFFER};
	vbo_position.data(V, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	vbo_normal = GLBuffer{GL_ARRAY_BUFFER};
	vbo_normal.data(Normal, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void init_surface(int m, int n, int p){
	vec3 pmin = {-2, -2, -2}; 
	vec3 pmax = { 2,  2,  2};

	std::vector<vec3> V = marchingCubes(treco, m, n, p, pmin, pmax);

	n_verts = V.size();

	std::vector<vec3> Normal(n_verts);
	for(int i = 0; i < n_verts; i++)
		Normal[i] = normal_treco(V[i].x, V[i].y, V[i].z);

	init_buffers(V, Normal);
}

void init_shader(){
	ShaderProgram shaderProgram{
		Shader{"PhongShader.vert", GL_VERTEX_SHADER},
		Shader{"PhongShader.frag", GL_FRAGMENT_SHADER}
	};
	glUseProgram(shaderProgram);

	Uniform{"Ka"} = vec3{0.7, 0.7, 1.0};
	Uniform{"Kd"} = vec3{0.7, 0.7, 1.0};
	Uniform{"Ks"} = vec3{ 1.0, 1.0, 1.0};
	Uniform{"shininess"} = 40.0f;

	Uniform{"light_position"} = vec4{ 0.0, 8.0, 10.0, 1.0 };
	Uniform{"Ia"} = vec3{ 0.2, 0.2, 0.2};
	Uniform{"Id"} = vec3{ 1.0, 1.0, 1.0};
	Uniform{"Is"} = vec3{ 1.0, 1.0, 1.0};
}

void init(){
	glewInit();
	glEnable(GL_DEPTH_TEST);
	
	init_shader();
	init_surface(150, 150, 150);
}

void setup_matrices(){
	Uniform{"Model"} = rotate_z(angle);
	Uniform{"View"} = lookAt({3, 3, 1.5}, {0, 0, 0}, {0, 0, 1});
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	float a = w/(float)h;
	Uniform{"Projection"} = scale(1,1,-1)*perspective(50, a, 0.1, 50);
}

void desenha(){
	glClearColor(1, 1, 1, 1);	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setup_matrices();

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, n_verts);

	glutSwapBuffers();
}

int last_x;

void mouse(int button, int state, int x, int y){
	last_x = x;
}

void mouseMotion(int x, int y){
	int dx = x - last_x;
	angle += dx*0.01;
	last_x = x;
	glutPostRedisplay();
}


int main(int argc, char* argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("janela");
	glutDisplayFunc(desenha);
	
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);

	printf("GL Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	init();
	glutMainLoop();
}
