#include <dm/dm.hpp>

using namespace std;
using namespace glm;

namespace dm {

// http://stackoverflow.com/questions/108318/whats-the-simplest-way-to-test-whether-a-number-is-a-power-of-2-in-c
static bool isPowerOfTwo(int n) {
    return n>0 && !(n & (n-1));
}

static SDL_Surface* SDL2Surface_vSymmetry(SDL_Surface *src) {

    hope(src->format->BitsPerPixel > 8);

    SDL_Surface *dst = SDL_CreateRGBSurface(
        0, src->w, src->h, 
        src->format->BitsPerPixel,
        src->format->Rmask,
        src->format->Gmask,
        src->format->Bmask,
        src->format->Amask
    );
    if(!dst) {
        cerr << "Failed to create dst : " <<  SDL_GetError() << endl;
        return nullptr;
    }
    SDL_Rect srcrect = { 0,        0, src->w, 1 };
    SDL_Rect dstrect = { 0, src->h-1, src->w, 1 };
    for(int y=0 ; y<src->h && dstrect.y != srcrect.y ; ++y, ++(srcrect.y), --(dstrect.y))
        SDL_BlitSurface(src, &srcrect, dst, &dstrect);
    return dst;
}



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

GLuint GLTexture_fromRgba32(rgba32 rgba) {
    return configureTexture(GL_RGBA, &rgba, 1, 1);
}

GLuint GLTexture_fromRgba32Image(const Image<rgba32> &img) {
    return configureTexture(GL_RGBA, img.pixels, img.w, img.h);
}

GLuint GLTexture_fromRgb24Image(const Image<rgb24> &img) {
    return configureTexture(GL_RGB, img.pixels, img.w, img.h);
}

GLuint GLTexture_fromFile(const string &filepath) {
    cout << "Loading `" << filepath << "'... ";
    cout.flush();
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
    
    SDL_Surface *s_ = SDL2Surface_vSymmetry(s);
    SDL_FreeSurface(s);
    if(!s_) {
        cerr << "Failed to allocate vertical symmetry!" << endl;
        return false;
    }
    texid = configureTexture(format, s_->pixels, s_->w, s_->h);
    SDL_FreeSurface(s_);
    cout << "done (id = " << texid << ")." << endl;
    return texid;
}

void GLTexture_bindToUnit(GLuint tex, TextureUnit unit) {
    glActiveTexture(GL_TEXTURE0 + size_t(unit));
    glBindTexture(GL_TEXTURE_2D, tex);
    glActiveTexture(GL_TEXTURE0);
    cout << "Texture id " << tex << " bound to unit " << size_t(unit) << "." << endl;
}

} // namespace dm
