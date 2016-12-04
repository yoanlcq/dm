// Repris en partie de GLimac

#pragma once

#include <dm/dm.hpp>

namespace dm {

struct GLShader {
	GLuint shader_id;
	GLShader(GLenum type): shader_id(glCreateShader(type)) {}
	~GLShader() { glDeleteShader(shader_id); }
	GLShader(GLShader&& rvalue): shader_id(rvalue.shader_id) {
		rvalue.shader_id = 0;
	}

	GLShader& operator =(GLShader&& rvalue) {
		shader_id = rvalue.shader_id;
		rvalue.shader_id = 0;
		return *this;
	}

	void setSource(const char* src) {
		glShaderSource(shader_id, 1, &src, 0);
	}
	bool compile();
	const std::string getInfoLog() const;
};

// Load a shader (but does not compile it)
GLShader loadGLShader(GLenum type, const std::string& filepath);


struct GLProgram {
    GLuint prog_id;
	GLProgram(): prog_id(glCreateProgram()) {}
	~GLProgram() {glDeleteProgram(prog_id);}

	GLProgram(GLProgram&& rvalue): prog_id(rvalue.prog_id) {
		rvalue.prog_id = 0;
	}

	GLProgram& operator =(GLProgram&& rvalue) {
		prog_id = rvalue.prog_id;
		rvalue.prog_id = 0;
		return *this;
	}



	void attachGLShader(const GLShader& shader) {
		glAttachShader(prog_id, shader.shader_id);
	}
	bool link();
	const std::string getInfoLog() const;
	void use() const {
		glUseProgram(prog_id);
	}
};
//// Build a GLSL program from source code
GLProgram buildGLProgram(const GLchar* vsSrc, const GLchar* fsSrc);
// Load source code from files and build a GLSL program
GLProgram loadGLProgram(const std::string& vsFile, const std::string& fsFile);

}
