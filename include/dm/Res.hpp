#pragma once

#include <dm/dm.hpp>

namespace dm {

// Struct managing the game's resources.
struct Res {
    bool setup();
    void cleanup();

    GLuint quadbatch_prog;
    GLuint welcome_screen_tex;

    static const std::string QUADBATCH_VERT;
    static const std::string QUADBATCH_FRAG;
    static const std::string WELCOME_SCREEN_PNG;
    static const std::string WORLD_MAP_PNG;
    static const std::string WORLD_MAP_TXT;
    static const std::string WORLD_MAP_HERO_PNG;
};

}
