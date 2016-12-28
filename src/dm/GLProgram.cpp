#include <dm/dm.hpp>

using namespace std;

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


bool GLShader::compile() {
	glCompileShader(shader_id);
	GLint status;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
	return status == GL_TRUE;
}

const string GLShader::getInfoLog() const {
	GLint length;
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
	char* log = new char[length];
	glGetShaderInfoLog(shader_id, length, 0, log);
	string logString(log);
	delete [] log;
	return logString;
}

static string readWholeFile(const string& filepath) {
    ifstream input(filepath.c_str());
    if(!input) {
        cerr << "Unable to load `" << filepath << "'." << endl;
        return "";
    }
    stringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

bool GLProgram_link(GLuint prog_id) {
	glLinkProgram(prog_id);
	GLint status;
	glGetProgramiv(prog_id, GL_LINK_STATUS, &status);
	return status == GL_TRUE;
}

const string GLProgram_getInfoLog(GLuint prog_id) {
	GLint length;
	glGetProgramiv(prog_id, GL_INFO_LOG_LENGTH, &length);
	char* log = new char[length];
	glGetProgramInfoLog(prog_id, length, 0, log);
	string logString(log);
	delete [] log;
	return logString;
}

static GLuint buildGLProgramNamed(const GLchar* vsSrc, 
                                  const string &vsName,
                                  const GLchar* fsSrc,
                                  const string &fsName) {
	GLShader vs(GL_VERTEX_SHADER);
	vs.setSource(vsSrc);
	GLShader fs(GL_FRAGMENT_SHADER);
	fs.setSource(fsSrc);

	if(!vs.compile()) {
        cerr << vsName << ": " << vs.getInfoLog() << endl;
        return 0;
	}
	if(!fs.compile()) {
        cerr << fsName << ": " << fs.getInfoLog() << endl;
        return 0;
	}

	GLuint program = glCreateProgram();
    if(!program) 
        return 0;
    glAttachShader(program, vs.shader_id);
    glAttachShader(program, fs.shader_id);

	if(!GLProgram_link(program))
        cerr << "Link error:" << endl
             << vsName << endl
             << fsName << endl
             << GLProgram_getInfoLog(program) << endl;

	return program;
}

GLuint GLProgram_fromSources(const GLchar* vsSrc, const GLchar* fsSrc) {
    return buildGLProgramNamed(vsSrc, "Vertex Shader", fsSrc, "Fragment Shader");
}

GLuint GLProgram_fromFiles(const std::string& vsFile, const std::string& fsFile) {
    cout << "Compiling `" << vsFile << "' and `" << fsFile << "'... ";
    cout.flush();
	GLuint prog = buildGLProgramNamed(
        readWholeFile(vsFile).c_str(), vsFile, 
        readWholeFile(fsFile).c_str(), fsFile
    );
    cout << "done." << endl;
    return prog;
}

} // namespace dm
