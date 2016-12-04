#include <dm/dm.hpp>

namespace dm {

struct GLTexture {
    GLuint texid;
    GLTexture(const std::string &filepath);
    GLTexture(uint32_t rgba=0xffcc00fful);
    ~GLTexture();
};

}

