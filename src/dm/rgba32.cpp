#include <dm/dm.hpp>

namespace dm {

rgba32::rgba32(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}
rgba32::~rgba32() {}

rgb24::rgb24(uint8_t r, uint8_t g, uint8_t b) : data{r, g, b} {}
rgb24::~rgb24() {}


}
