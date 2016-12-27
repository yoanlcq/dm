#include <dm/dm.hpp>

using namespace std;
using namespace glm;

namespace dm {

// http://stackoverflow.com/questions/108318/whats-the-simplest-way-to-test-whether-a-number-is-a-power-of-2-in-c
static bool isPowerOfTwo(int n) {
    return n>0 && !(n & (n-1));
}

static GLuint configureTexture(GLenum format, const void *pixels, size_t w, size_t h) {
    GLuint texid = 0;
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, 
                 format, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texid;
}

GLuint GLTexture_fromRgba32(rgba32 rgba) {
    return configureTexture(GL_RGBA, &rgba, 1, 1);
}

GLuint GLTexture_fromFile(const string &filepath) {
    GLuint texid = 0;
    SDL_Surface *s = IMG_Load(filepath.c_str());
    if(!s) {
        cerr << "Could not load `" << filepath << "' ! ("
             << "IMG_Load(): " << IMG_GetError() << ")" << endl;
        return 0;
    }
    GLenum format;
    switch(s->format->BytesPerPixel) {
    case 3: format = GL_RGB;  break;
    case 4: format = GL_RGBA; break;
    default: 
        cerr << filepath << ": Not enough bytes per pixel !" << endl; 
        return 0;
    }
    if(s->w != s->h)
        cerr << "Warning : Texture `" << filepath << "' is not square!" << endl;
    if(!(isPowerOfTwo(s->w) && isPowerOfTwo(s->h)))
        cerr << "Warning : Texture `" << filepath << "' is not power of two!" << endl;
    
    texid = configureTexture(format, s->pixels, s->w, s->h);
    SDL_FreeSurface(s);
    return texid;
}

} // namespace dm
