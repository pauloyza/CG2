#include <GL/glew.h>
#include <GL/freeglut.h>
#include "matrix.h"
#include "Color.h"
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

class GLMesh{
	VAO vao;
	GLBuffer vbo;
	std::vector<MaterialRange> materials;
	std::map<std::string, GLTexture> texture_map;
	public:
	mat4 Model;

	GLMesh() = default;

	GLMesh(std::string obj_file, mat4 _Model, std::string default_texture = ""){
		ObjMesh mesh{obj_file};
		init_buffers(mesh.getTriangles());

		MaterialInfo std_mat;
		std_mat.map_Kd = default_texture;

		materials = mesh.getMaterials(std_mat);

		for(MaterialRange range: materials)
			load_texture(mesh.path, range.mat.map_Kd);
		
		Model = _Model;
	}

	void draw() const{
		glBindVertexArray(vao);
		for(MaterialRange range: materials){
			get_texture(range.mat.map_Kd);
			glDrawArrays(GL_TRIANGLES, range.first, range.count);
		}
	}

	private:
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
	
	void get_texture(std::string file) const{
		auto it = texture_map.find(file);
		if(it != texture_map.end())
			glBindTexture(GL_TEXTURE_2D, it->second);
	}
};

ShaderProgram shaderProgram;
std::vector<GLMesh> meshes;
float vangle = 0;
mat4 BaseView = lookAt({0, 1.6, 5}, {0, 1.6, 0}, {0, 1, 0});

void init_shader(){
	shaderProgram = ShaderProgram{
		Shader{"TextureShader.vert", GL_VERTEX_SHADER},
		Shader{"TextureShader.frag", GL_FRAGMENT_SHADER}
	};
	glUseProgram(shaderProgram);
}

void init(){
	glewInit();
	glEnable(GL_DEPTH_TEST);
	
	init_shader();
	
	meshes.emplace_back("modelos/floor.obj", 
		scale(35, 35, 35), "../brickwall.jpg");

	meshes.emplace_back("modelos/box.obj", 
		translate(1, 1, -6)*rotate_y(-0.4), "../bob.jpg");

	meshes.emplace_back("modelos/metroid/DolBarriersuit.obj", 
		translate(-1, 0, -1)*rotate_y(0.2)*scale(.1, .1, .1));

	meshes.emplace_back("modelos/pose/pose.obj", 
		translate(1, 0, 0)*scale(0.009, 0.009, 0.009));

	meshes.emplace_back("modelos/House Complex/House Complex.obj", 
		translate(4, 0, 0)*rotate_y(0.5*M_PI)*scale(.15, .15, .15));

	meshes.emplace_back("modelos/pony-cartoon/Pony_cartoon.obj", 
		translate(-2, -0.0001, -3)*scale(0.005, 0.005, 0.005), "brickwall.jpg" );
}

void desenha(){
	glClearColor(0, 165.0/255, 220.0/255, 1);	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	float a = w/(float)h;

	mat4 Projection = scale(1,1,-1)*perspective(45, a, 0.1, 100);
	mat4 View = rotate_x(vangle)*BaseView;

	for(const GLMesh& mesh: meshes){
		mat4 Model = mesh.Model;
		Uniform{"M"} = Projection*View*Model;
		mesh.draw();
	}

	glutSwapBuffers();
}

int last_x, last_y;
void mouse(int button, int state, int x, int y){
	last_x = x;
	last_y = y;
}

void mouseMotion(int x, int y){
	int dx = x - last_x;
	int dy = y - last_y;

	vangle += 0.01*dy;
	vangle = clamp(vangle, -1.5, 1.5);
	BaseView = rotate_y(dx*0.01)*BaseView;

	last_x = x;
	last_y = y;	
	glutPostRedisplay();
}

void special(int key, int x, int y){
	vec3 dir = {0, 0, 0};
	float zmove = 0;
	float xmove = 0;

	if(key == GLUT_KEY_UP)
		zmove += 0.2;

	if(key == GLUT_KEY_DOWN)
		zmove -= 0.2;

	if(key == GLUT_KEY_LEFT)
		xmove += 0.2;

	if(key == GLUT_KEY_RIGHT)
		xmove -= 0.2;

	BaseView = translate(xmove, 0, zmove)*BaseView;
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
	glutSpecialFunc(special);

	printf("GL Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	init();
	glutMainLoop();
}
