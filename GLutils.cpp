#include <GL/glew.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "GLutils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

////////////////////////////////////////////////////////////////////
void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

void enable_debug(){
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
}
	
////////////////////////////////////////////////////////////////////
std::string loadSource(std::string file){
	std::ifstream in(file);
	std::stringstream buffer;
	buffer << in.rdbuf();
	return buffer.str();
}

Shader::Shader(std::string file, GLenum shader_type){
	type = shader_type;

	std::string s = loadSource(file);
	const char* source = s.c_str();

	id = glCreateShader(type);
	glShaderSource(id, 1, &source, NULL);
	glCompileShader(id);

	deleter = glDeleteShader;
	
	check();	
}

void Shader::check(){
	int  success;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if(success == GL_FALSE){
		GLint maxLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

		char infoLog[maxLength];
		glGetShaderInfoLog(id, maxLength, NULL, infoLog);
		std::string shaderType = 
			(type==GL_VERTEX_SHADER)? "VERTEX": 
			(type==GL_FRAGMENT_SHADER)? "FRAGMENT": 
			(type==GL_GEOMETRY_SHADER)? "GEOMETRY": 
			(type==GL_TESS_CONTROL_SHADER)? "TESSELATION CONTROL": 
			(type==GL_TESS_EVALUATION_SHADER)? "TESSELATION EVALUATION": 
			(type==GL_COMPUTE_SHADER)? "COMPUTE": 
			"UNKNOWN";
		std::cerr << "ERROR::SHADER::" << shaderType << 
			"::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}
	
////////////////////////////////////////////////////////////////////
void ShaderProgram::check(){
	GLint success = 0;
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if(success == GL_FALSE){
		GLint maxLength = 0;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		char infoLog[maxLength];
		glGetProgramInfoLog(id, maxLength, NULL, infoLog);

		std::cerr << "ERROR::LINK_FAILED\n" << infoLog << '\n';
		// The program is useless now. So delete it.
		glDeleteProgram(id);
	}
}

////////////////////////////////////////////////////////////////////
void load_texture_data(GLenum target, std::string filename){
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

	if(data == NULL){
		std::cout << "ERROR: could not read texture " << filename << '\n';
		return;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	int format[] = {0, GL_RED, GL_RG, GL_RGB, GL_RGBA};
	glTexImage2D(target, 0, GL_RGBA, width, height, 0, 
		format[nrChannels], GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
}

////////////////////////////////////////////////////////////////////
void GLTexture::load(std::string filename, GLenum target){
	if(target == 0)
		target = this->target;

	glBindTexture(target, id);

	bool flip = (target == GL_TEXTURE_2D);
	stbi_set_flip_vertically_on_load(flip);
	load_texture_data(target, filename);
}
/*

unsigned int loadCubemap(std::string texture_faces[6]){
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	
	stbi_set_flip_vertically_on_load(false);
	for(int i = 0; i < 6; i++)
		load_texture_data(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, texture_faces[i]);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  

	return textureID;
}
*/
