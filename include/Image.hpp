#pragma once
#include <dm/dm.hpp>
namespace dm {

struct PPMImage : public Image<rgba32> {
    static bool loadFromFile(const std::string &filepath);
};

}
