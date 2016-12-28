#pragma once

namespace dm {

enum class TextureUnit {
    INVALID_UNIT   = 0, // Keep it to zero
    WELCOME_SCREEN,
    WORLD_MAP_BG,
    WORLD_MAP_HERO,
    WORLD_MAP_FADER,

    // Reserve other texture units here...

    COUNT // Keep last
};

}
