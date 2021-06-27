#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Color.h"
#include "GLutils.h"
#include "MarchingCubes.h"

ShaderProgram shaderProgram;
VAO vao;
GLBuffer vbo_position, vbo_normal;
int n_verts;
float angle = 0;

float T = 0.5;

struct Metaball{
	float a;
	float b;
	vec3 center;

	float operator()(vec3 P){
		float r = norm(P-center);

		return r >= b? 0.0f:
		       r >= b/3? 3*a/2*pow(1-r/b, 2):
		       a*(1-3*r*r/(b*b));
	}

	vec3 normal(vec3 P){
		vec3 d = P - center;
		float r = norm(d);
		return r >= b? vec3{0, 0, 0}:
		       r >= b/3? (-3*a/b)*(-normalize(d) + 1/b*d):
		       (6*a/b)*d;
	}
};

struct MetaballFigure{
	float T;
	std::vector<Metaball> metaballs;

	float operator()(float x, float y, float z){
		vec3 P = {x, y, z};

		float sum = 0;
		for(Metaball fi: metaballs)
			sum += fi(P);

		return T - sum;
	}

	vec3 normal(vec3 P){
		vec3 n = {0, 0, 0};

		for(Metaball fi: metaballs)
			n = n + fi.normal(P);

		return normalize(n);
	}
};

MetaballFigure figure;

void init_buffers(){
	vao = VAO{true};
	glBindVertexArray(vao);

	vbo_position = GLBuffer{GL_ARRAY_BUFFER};
	vbo_normal = GLBuffer{GL_ARRAY_BUFFER};
}

void update_buffers(const std::vector<vec3>& V, const std::vector<vec3>& Normal){
	vbo_position.data(V, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	vbo_normal.data(Normal, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void update_surface(){
	figure.T = T;
	figure.metaballs = {
		{1, 1.2, { 0, 0, 0}},

		{1, 0.5, { 0.7, 0, 0}},
		{1, 0.5, {-0.7, 0, 0}},
		{1, 0.8, {0.4, 0,0.9}},
		{1, 0.8, {-0.4, 0,0.9}},

		{1, 0.3, {0, 0.3,0}},

		{1, 0.3, {0.25, 0.3,0.2}},
		{1, 0.3, {-0.25, 0.3,0.2}},

		{1, 0.2, {0, 0.25,-0.5}}
	};

	vec3 pmin = {-2, -2, -2};
	vec3 pmax = { 2,  2,  2};
	int m = 90, n = 90, p = 90;

	std::vector<vec3> V = marchingCubes(figure, m, n, p, pmin, pmax);

	n_verts = V.size();

	std::vector<vec3> Normal(n_verts);
	for(int i = 0; i < n_verts; i++)
		Normal[i] = figure.normal(V[i]);

	update_buffers(V, Normal);
}

void init_shader(){
	shaderProgram = ShaderProgram{
		Shader{"PhongShader.vert", GL_VERTEX_SHADER},
		Shader{"PhongShader.frag", GL_FRAGMENT_SHADER}
	};
	glUseProgram(shaderProgram);

	Uniform{"Ka"} = vec3{2.7, 0.7, 1.5};
	Uniform{"Kd"} = vec3{0.7, 0.7, 1.0};
	Uniform{"Ks"} = vec3{ 1.0, 1.0, 1.0};
	Uniform{"shininess"} = 40.0f;

	Uniform{"light_position"} = vec4{ 10.0, 10.0, 5.0, 1.0 };
	Uniform{"Ia"} = vec3{ 0.2, 0.2, 0.2};
	Uniform{"Id"} = vec3{ 1.0, 1.0, 1.0};
	Uniform{"Is"} = vec3{ 1.0, 1.0, 1.0};
}

void init(){
	glewInit();
	glEnable(GL_DEPTH_TEST);

	init_shader();
	init_buffers();
	update_surface();
}

void setup_matrices(){
	Uniform{"Model"} = rotate_z(angle);
	Uniform{"View"} = lookAt({3, 3, 0}, {0, 0, 0}, {0, 0, 1});
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	float a = w/(float)h;
	Uniform{"Projection"} = scale(1,1,-1)*perspective(50, a, 0.1, 50);
}

void desenha(){
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setup_matrices();
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

void keyboard(unsigned char c, int x, int y){
	if(c == 'a'){
		T *= 1.1;
		update_surface();
	}
	if(c == 'A'){
		T /= 1.1;
		update_surface();
	}
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

	glutKeyboardFunc(keyboard);

	printf("GL Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	init();
	glutMainLoop();
}
