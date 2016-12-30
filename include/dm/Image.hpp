#pragma once
#include <dm/dm.hpp>

namespace dm {

template<typename T>
struct Image {
    T *pixels;
    size_t w, h;
     Image() : pixels(nullptr), w(0), h(0) {}
    ~Image() { free(pixels); }
    void resize(size_t nw, size_t nh) {
        pixels = (T*) realloc(pixels, nw*nh*sizeof(T));
        hope(pixels);
        w = nw;
        h = nh;
    }
    T *operator[](int y) const { return &pixels[y*w]; }
    void flipVertically() {
        T *line = new T[w];
        for(int yh=0, yl=h-1 ; yh < yl ; ++yh, --yl) {
            memcpy(line,        (*this)[yl], w*sizeof(T));
            memcpy((*this)[yl], (*this)[yh], w*sizeof(T));
            memcpy((*this)[yh], line,        w*sizeof(T));
        }
        delete[] line;
    }
};

}
