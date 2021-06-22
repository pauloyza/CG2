#include <GL/glew.h>
#include <GL/freeglut.h>
#include "GLutils.h"
#include "ObjMesh.h"

VAO vao;
GLBuffer vbo;
int n_verts;

ShaderProgram shaderProgram;

float angle = 0;

void init_shader(){
	shaderProgram = ShaderProgram{
		Shader{"PhongShader.vert", GL_VERTEX_SHADER},
		Shader{"PhongShader.frag", GL_FRAGMENT_SHADER}
		//Shader{"GouraudShader.vert", GL_VERTEX_SHADER},
		//Shader{"ColorShader.frag", GL_FRAGMENT_SHADER}
	};
}

void init_surface(){
	ObjMesh mesh{"modelos/teapot.obj"};
	auto mesh_triangles = mesh.getTriangles();
	
	vao = VAO{true};
	glBindVertexArray(vao);

	vbo = GLBuffer{GL_ARRAY_BUFFER};
	vbo.data(mesh_triangles, GL_STATIC_DRAW);

	size_t stride = sizeof(ObjMesh::Vertex);
	size_t offset_position = offsetof(ObjMesh::Vertex, position);
	size_t offset_normal = offsetof(ObjMesh::Vertex, normal);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset_position);
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,(void*)offset_normal);

	n_verts = mesh_triangles.size();
}

void init(){
	glewInit();
	glEnable(GL_DEPTH_TEST);
	
	init_shader();
	init_surface();
}

void desenha(){
	glClearColor(1, 1, 1, 1);	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 Model = rotate_y(angle)*rotate_x(-M_PI/2);
	mat4 View = lookAt({23, 23, 15}, {0.2, 8, 0}, {0, 1, 0});
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	float a = w/(float)h;
	mat4 Projection = scale(1,1,-1)*perspective(45, a, 0.1, 50);

	glUseProgram(shaderProgram);
	Uniform{"Projection"} = Projection;
	Uniform{"View"} = View;
	Uniform{"Model"} = Model;
	
	Uniform{"light_position"} = vec4{ 0, 13, 15, 1.0};
	Uniform{"Ia"} = vec3{ 0.2, 0.2, 0.2};
	Uniform{"Id"} = vec3{ 1.0, 1.0, 1.0};
	Uniform{"Is"} = vec3{ 1.0, 1.0, 1.0};
		
	Uniform{"Ka"} = vec3{1, 1, 1};
	Uniform{"Kd"} = vec3{0.5, 0.5, 0.5};
	Uniform{"Ks"} = vec3{0.5, 0.5, 0.5};
	Uniform{"shininess"} = 128.0f;

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
	glutInitWindowSize(800, 600);
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
