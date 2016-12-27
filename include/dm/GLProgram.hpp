// Repris en partie de GLimac.

#pragma once

#include <dm/dm.hpp>

namespace dm {

GLuint GLProgram_fromSources(const GLchar* vsSrc, const GLchar* fsSrc);
GLuint GLProgram_fromFiles(const std::string& vsFile, const std::string& fsFile);

}
