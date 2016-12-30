#include <dm/dm.hpp>

namespace dm {

GLuint GLTexture_fromRgba32(rgba32 rgba=rgba32(0xff, 0xcc, 0x00, 0xff));
GLuint GLTexture_fromFile(const std::string &filepath);
GLuint GLTexture_fromRgb24Image(const Image<rgb24> &img);
GLuint GLTexture_fromRgba32Image(const Image<rgba32> &img);
void   GLTexture_bindToUnit(GLuint tex, TextureUnit unit);

}

