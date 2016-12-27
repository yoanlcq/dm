#include <dm/dm.hpp>

using namespace std;

namespace dm {

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

bool GLProgram::link() {
	glLinkProgram(prog_id);
	GLint status;
	glGetProgramiv(prog_id, GL_LINK_STATUS, &status);
	return status == GL_TRUE;
}

const string GLProgram::getInfoLog() const {
	GLint length;
	glGetProgramiv(prog_id, GL_INFO_LOG_LENGTH, &length);
	char* log = new char[length];
	glGetProgramInfoLog(prog_id, length, 0, log);
	string logString(log);
	delete [] log;
	return logString;
}

static GLProgram buildGLProgramNamed(const GLchar* vsSrc, 
                              const string &vsName,
                              const GLchar* fsSrc,
                              const string &fsName) {
	GLProgram program;
	GLShader vs(GL_VERTEX_SHADER);
	vs.setSource(vsSrc);
	GLShader fs(GL_FRAGMENT_SHADER);
	fs.setSource(fsSrc);

	if(!vs.compile()) {
        cerr << vsName << ": " << vs.getInfoLog() << endl;
        return program;
	}
	if(!fs.compile()) {
        cerr << fsName << ": " << fs.getInfoLog() << endl;
        return program;
	}

	program.attachGLShader(vs);
	program.attachGLShader(fs);

	if(!program.link())
        cerr << "Link error:" << endl
             << vsName << endl
             << fsName << endl
             << program.getInfoLog() << endl;

	return program;
}

// Build a GLSL program from source code
GLProgram buildGLProgram(const GLchar* vsSrc, const GLchar* fsSrc) {
    return buildGLProgramNamed(vsSrc, "Vertex Shader", fsSrc, "Fragment Shader");
}

// Load source code from files and build a GLSL program
GLProgram loadGLProgram(const string& vsFile, const string& fsFile) {
	return buildGLProgramNamed(
        readWholeFile(vsFile).c_str(), vsFile, 
        readWholeFile(fsFile).c_str(), fsFile
    );
}

}
