#pragma once
#include <dm/dm.hpp>
namespace dm {

struct PPMImage : public Image<rgb24> {
    bool loadFromFile(const std::string &filepath);
};

}
