#ifndef GLSL_UTILS_H
#define GLSL_UTILS_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <array>
#include <functional>

#include "vec.h"
#include "matrix.h"
#include "Color.h"

////////////////////////////////////////////////////////////////////
void enable_debug();

////////////////////////////////////////////////////////////////////
inline int currentProgram(){
	int program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);	
	return program;
}

////////////////////////////////////////////////////////////////////

struct Uniform{
	int loc = -1;

	Uniform() = default;

	Uniform(std::string var){
		loc = glGetUniformLocation(currentProgram(), var.c_str());
	}

	Uniform& operator=(int v){
		glUniform1i(loc, v);
		return *this;
	}

	Uniform& operator=(float v){
		glUniform1f(loc, v);
		return *this;
	}

	Uniform& operator=(vec2 v){
		glUniform2fv(loc, 1, &v.x);
		return *this;
	}

	Uniform& operator=(vec3 v){
		glUniform3fv(loc, 1, &v.x);
		return *this;
	}

	Uniform& operator=(vec4 v){
		glUniform4fv(loc, 1, &v.x);
		return *this;
	}

	Uniform& operator=(mat2 M){
		glUniformMatrix2fv(loc, 1, true, &M[0][0]);
		return *this;
	}

	Uniform& operator=(mat3 M){
		glUniformMatrix3fv(loc, 1, true, &M[0][0]);
		return *this;
	}

	Uniform& operator=(mat4 M){
		glUniformMatrix4fv(loc, 1, true, &M[0][0]);
		return *this;
	}

	Uniform& operator=(const std::vector<float>& v){
		glUniform1fv(loc, v.size(), v.data());
		return *this;
	}
	
	template<size_t N>
	Uniform& operator=(const std::array<vec3, N>& v){
		glUniform3fv(loc, v.size(), &v[0].x);
		return *this;
	}
};

////////////////////////////////////////////////////////////////////
struct UintResource{
	unsigned int id = 0;
	std::function<void(unsigned int)> deleter;

	operator unsigned int() const{ return id; }

	UintResource() = default;

	~UintResource(){
		if(id && deleter)
			deleter(id);
	}


	UintResource(UintResource&& other){
		id = other.id;
		other.id = 0;
	}

	UintResource& operator=(UintResource&& other){
		if(&other != this){
			UintResource tmp{std::move(other)};
			std::swap(id, tmp.id);
		}
		return *this;
	}
};

////////////////////////////////////////////////////////////////////
struct Shader : public UintResource{
	GLenum type;
	Shader() = default;
	Shader(std::string file, GLenum shader_type);
	void check();
};

////////////////////////////////////////////////////////////////////
struct ShaderProgram : public UintResource{
	ShaderProgram() = default;
	
	template<class...T>
	ShaderProgram(const T&... shaders){
		id = glCreateProgram();
		deleter = glDeleteProgram;
		init(shaders...);
	}

	int getAttribLocation(std::string attrib){
		return glGetAttribLocation(id, attrib.c_str());
	}
	private:
	void init(){
		glLinkProgram(id);
		check();
	}
	template<class...T>
	void init(const Shader& s, const T&...tail){
		glAttachShader(id, s);
		init(tail...);
		glDetachShader(id, s);
	}


	void check();
};

////////////////////////////////////////////////////////////////////
struct GLBuffer : public UintResource{
	GLenum type;

	GLBuffer() = default;

	GLBuffer(GLenum type) : type{type}{
		glGenBuffers(1, &id);
		deleter = [](unsigned int id){
			glDeleteBuffers(1, &id);
		};
	}

	template<class T>
	void data(const std::vector<T>& V, GLenum usage){
		glBindBuffer(type, id);
		glBufferData(type, V.size()*sizeof(T), V.data(), usage);
	}
};

////////////////////////////////////////////////////////////////////
struct VAO : public UintResource{
	VAO(bool init = false){
		if(!init)
			return;

		glGenVertexArrays(1, &id);
		
		deleter = [](unsigned int id){
			glDeleteVertexArrays(1, &id);
		};
	}
};

////////////////////////////////////////////////////////////////////
struct GLTexture : public UintResource{
	GLenum target;

	GLTexture() = default;

	GLTexture(GLenum target) : target{target}{
		glGenTextures(1, &id);
		glBindTexture(target, id);
		deleter = [](unsigned int id){
			glDeleteTextures(1, &id);
		};
	}

	void load(std::string filename, GLenum target = 0);
};

#endif
