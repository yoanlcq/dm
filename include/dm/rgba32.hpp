#pragma once
#include <dm/dm.hpp>
namespace dm {

struct rgba32 {
    uint32_t r:8, g:8, b:8, a:8;
    rgba32(uint8_t r=0, uint8_t g=0, uint8_t b=0, uint8_t a=0);
    ~rgba32();
};

}
