#include <GL/glew.h>
#include <GL/freeglut.h>
#include "GLutils.h"
#include "ObjMesh.h"

using Vertex = ObjMesh::Vertex;

GLTexture init_texture(std::string image){
	GLTexture texture{GL_TEXTURE_2D};
	texture.load(image);

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if(GLEW_EXT_texture_filter_anisotropic){
		GLfloat fLargest;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
	}
	return texture;
}

MaterialInfo standard_material(std::string mat_Kd){
	MaterialInfo mat;

	mat.name = "standard";

	mat.Ka = {1, 1, 1};
	mat.Kd = {1, 1, 1};
	mat.Ks = {0, 0, 0};
	mat.Ns = 1;

	mat.map_Kd = mat_Kd;

	return mat;
}

struct SurfaceMesh{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

class GLMesh{
	VAO vao;
	GLBuffer vbo;
	GLBuffer ebo;
	std::vector<MaterialRange> materials;
	std::map<std::string, GLTexture> texture_map;
	public:
	mat4 Model;

	GLMesh() = default;

	GLMesh(std::string obj_file, mat4 _Model, MaterialInfo std_mat=standard_material("")){
		ObjMesh mesh{obj_file};
		std::vector<Vertex> tris = mesh.getTriangles();
		init_buffers(tris);

		materials = mesh.getMaterials(std_mat);

		for(MaterialRange range: materials){
			load_texture(mesh.path, range.mat.map_Ka);
			load_texture(mesh.path, range.mat.map_Kd);
			load_texture(mesh.path, range.mat.map_Ks);
			load_texture(mesh.path, range.mat.map_Bump);
		}
		
		Model = _Model;
	}
	
	GLMesh(const SurfaceMesh& surface, mat4 _Model, MaterialInfo std_mat = standard_material("")){
		Model = _Model;
		init_buffers(surface.vertices);

		ebo = GLBuffer{GL_ELEMENT_ARRAY_BUFFER};
		ebo.data(surface.indices, GL_STATIC_DRAW);

		unsigned int size = surface.indices.size();

		materials = {
			{std_mat, 0, size}
		};
		load_texture("", std_mat.map_Kd);
	}

	void init_buffers(const std::vector<Vertex>& vertices){
		vao = VAO{true};
		glBindVertexArray(vao);

		vbo = GLBuffer{GL_ARRAY_BUFFER};
		vbo.data(vertices, GL_STATIC_DRAW);

		size_t stride = sizeof(Vertex);
		size_t offset_position = offsetof(Vertex, position);
		size_t offset_texCoords = offsetof(Vertex, texCoords);
		size_t offset_normal = offsetof(Vertex, normal);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset_position);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,(void*)offset_texCoords);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,(void*)offset_normal);
	}
	
	void load_texture(std::string path, std::string file){
		if(file != "" && texture_map.find(file) == texture_map.end()){
			std::string img = path + file;
			std::cout << "read image " << img << '\n';
			texture_map[file] = init_texture(img);
		}
	}

	void draw(MaterialRange range) const{
		Uniform{"Ka"} = range.mat.Ka; 
		Uniform{"Kd"} = range.mat.Kd;
		Uniform{"Ks"} = range.mat.Ks;
		Uniform{"shininess"} = range.mat.Ns;

		bool has_map_Ka = texture_map.find(range.mat.map_Ka) != texture_map.end();
		Uniform{"has_map_Ka"} = has_map_Ka;
		if(has_map_Ka){
			Uniform{"map_Ka"} = 0;
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture_map.at(range.mat.map_Ka));
		}

		bool has_map_Kd = texture_map.find(range.mat.map_Kd) != texture_map.end();
		Uniform{"has_map_Kd"} = has_map_Kd;
		if(has_map_Kd){
			Uniform{"map_Kd"} = 1;
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture_map.at(range.mat.map_Kd));
		}

		bool has_map_Ks = texture_map.find(range.mat.map_Ks) != texture_map.end();
		Uniform{"has_map_Ks"} = has_map_Ks;
		if(has_map_Ks){
			Uniform{"map_Ks"} = 2;
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, texture_map.at(range.mat.map_Ks));
		}

		glBindVertexArray(vao);
		if(ebo == 0)
			glDrawArrays(GL_TRIANGLES, range.first, range.count);
		else
			glDrawElements(GL_TRIANGLES, range.count, GL_UNSIGNED_INT, (void*)(range.first*sizeof(int)));
	}

	void draw() const{
		Uniform{"Model"} = Model;
		for(MaterialRange range: materials)
			draw(range);
	}
};

SurfaceMesh flag_mesh(int m, int n){
	int N = m*n;

	float u0 = -5, u1 = 5, du = (u1-u0)/(m-1),
	      v0 = -5, v1 = 5, dv = (v1-v0)/(n-1);

	SurfaceMesh res;
	res.vertices.resize(N);
	for(int i = 0; i < m; i++){
		for(int j = 0; j < n; j++){
			float u = u0 + i*du;
			float v = v0 + j*dv;

			Vertex& V = res.vertices[i + j*m];
			V.position = {u, v, sin(u*v/4)};

			float s = i/(m-1.0);
			float t = j/(n-1.0);

			V.texCoords = {s, t};

			vec3 du = {1, 0, cos(u*v/4)*v/4};
			vec3 dv = {0, 1, cos(u*v/4)*u/4};
			V.normal = normalize(cross(du, dv));
		}
	}

	for(int i = 0; i < m-1; i++){
		for(int j = 0; j < n-1; j++){	
			unsigned int ij = i + j*m;
			res.indices.insert(res.indices.end(), {
					ij, ij+1, ij+m,
					ij+m+1, ij+m, ij+1
					});
		}
	}

	return res;
}

std::vector<GLMesh> meshes;
VAO vao_quad;
GLBuffer vbo_quad;

ShaderProgram shaderProgram;
ShaderProgram shaderProgramStencil;

float angle = 0;
	
void init_scene(){
	meshes.emplace_back(
		flag_mesh(50, 50), 
		translate(0, 8, -10)*scale(1.4282, 1, 1), 
		standard_material("brasil.png")
	);

	meshes.emplace_back(
		"modelos/bunny.obj", 
		translate(0, 5.2, 2), 
		standard_material("../blue.png")
	); 

	meshes.emplace_back(
		"modelos/monkey.obj", 
		translate(0, 5.6, -2)*scale(1.4, 1.4, 1.4)*rotate_x(-0.7)
	);

	meshes.emplace_back(
		"modelos/teapot.obj", 
		translate(6,0,4)*scale(.14,.14,.14)*rotate_x(-M_PI/2), 
		standard_material("../bob.jpg")
	);

	meshes.emplace_back(
		"modelos/wall.obj", 
		scale(20, 20, 20), 
		standard_material("../brickwall.jpg")
	);

	meshes.emplace_back(
		"modelos/Wood Table/Old Wood Table.obj", 
		translate(0,1.08,0)
	);

	meshes.emplace_back(
		"modelos/pose/pose.obj", 
		translate(-6, 0, 4)*rotate_y(1)*scale(.05, .05, .05)
	);

	meshes.emplace_back(
		"modelos/train-toy-cartoon/train-toy-cartoon.obj", 
		translate(0,0,6)*rotate_y(-2.3)*scale(120, 120, 120)
	);
}

void draw_scene(){
	for(GLMesh& m: meshes)
		m.draw();
}

void init_shader(){
	shaderProgram = ShaderProgram{
		Shader{"PhongShaderTex.vert", GL_VERTEX_SHADER},
		Shader{"PhongShaderTex.frag", GL_FRAGMENT_SHADER}
	};
	glUseProgram(shaderProgram);

	Uniform{"light_position"} = vec4{ 0.0, 8.0, 10.0, 1.0 };
	Uniform{"Ia"} = vec3{ 0.2, 0.2, 0.2};
	Uniform{"Id"} = vec3{ 1.0, 1.0, 1.0};
	Uniform{"Is"} = vec3{ 1.0, 1.0, 1.0};

	shaderProgramStencil = ShaderProgram{
		Shader{"stencil01.vert", GL_VERTEX_SHADER}
	};
}

void init_quad(){
	vao_quad = VAO{true};
	std::vector<vec2> V = {
		{-0.67, -0.66},
		{0.72, -0.33},
		{0.78, 0.24},
		{-0.85, 0.49}
	};
	
	vao_quad = VAO{true};
	glBindVertexArray(vao_quad);

	vbo_quad = GLBuffer{GL_ARRAY_BUFFER};
	vbo_quad.data(V, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void init(){
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	init_scene();
	init_quad();

	init_shader();
}

void setup_matrices(){
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	float a = w/(float)h;
	Uniform{"Projection"} = scale(1,1,-1)*perspective(45, a, 0.1, 50);
	vec4 pos = rotate_y(angle)*vec4{0, 8, 20, 1};
	Uniform{"View"} = lookAt(toVec3(pos), {0, 4, 0}, {0, 1, 0});
}

void draw_color_buffer(){
	glClearColor(1, 1, 1, 1);	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(shaderProgram);
	setup_matrices();
	draw_scene();
}

void draw_stencil_buffer(){
	glStencilMask(0xFF);
	glDepthMask(GL_FALSE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glClear(GL_STENCIL_BUFFER_BIT);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

	glUseProgram(shaderProgramStencil);
	glBindVertexArray(vao_quad);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

void desenha(){
	draw_stencil_buffer();
	draw_color_buffer();
	glutSwapBuffers();
}

int last_x;

void mouse(int button, int state, int x, int y){
	last_x = x;
}

void mouseMotion(int x, int y){
	int dx = x - last_x;
	angle -= dx*0.01;
	last_x = x;
	glutPostRedisplay();
}

int main(int argc, char* argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE | GLUT_DEPTH | GLUT_STENCIL);
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
