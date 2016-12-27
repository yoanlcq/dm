#include <dm/dm.hpp>

using namespace glm;

namespace dm {

GLuint GLMonospaceText::texture = 0;
vec2 GLMonospaceText::atlas_char_size(0,0);

void GLMonospaceText::setupGL() {}
void GLMonospaceText::cleanupGL() {}
void GLMonospaceText::updateQuadBatch() {}
void GLMonospaceText::renderGL() const {}

} // namespace dm
