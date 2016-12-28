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
        pixels = (T*) realloc(pixels, nw*nh);
        hope(pixels);
        w = nw;
        h = nh;
    }
    T *operator[](int y) { return &pixels[y*w]; }
};

}
