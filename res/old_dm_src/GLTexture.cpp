#include <dm/dm.hpp>

using namespace std;
using namespace glm;

namespace dm {

static GLuint configureTexture(GLenum format, const void *pixels, size_t w, size_t h) {
    GLuint texid = 0;
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, 
                 format, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texid;
}

GLTexture::GLTexture(uint32_t rgba) {
    // Endianness issue
    uint8_t rgba8[4] = {
      (uint8_t)((rgba & 0xff000000) >> 24),
      (uint8_t)((rgba & 0x00ff0000) >> 16),
      (uint8_t)((rgba & 0x0000ff00) >> 8),
      (uint8_t)((rgba & 0x000000ff))
    };
    texid = configureTexture(GL_RGBA, rgba8, 1, 1);
}

GLTexture::GLTexture(const string &filepath) {
    texid = 0;
    SDL_Surface *s = IMG_Load(filepath.c_str());
    if(!s) {
        cerr << "Could not load `" << filepath << "' ! ("
             << "IMG_Load(): " << IMG_GetError() << ")" << endl;
        return;
    }
    GLenum format;
    switch(s->format->BytesPerPixel) {
    case 3: format = GL_RGB;  break;
    case 4: format = GL_RGBA; break;
    default: cerr << filepath << ": Not enough bytes per pixel !" << endl; 
        return;
    }
    texid = configureTexture(format, s->pixels, s->w, s->h);

    SDL_FreeSurface(s);
}

GLTexture::~GLTexture() {
    glDeleteTextures(1, &texid);
}

}
