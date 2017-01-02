#pragma once

namespace dm {

enum class TextureUnit {
    INVALID_UNIT   = 0, // Keep it to zero
    MONOSPACE_FONT_ATLAS,
    WELCOME_SCREEN,
    WORLD_MAP,
    BLACK_FADER,
    TRANS_ATLAS,
    GRASS_GROUND,
    GRASS_WALL,
    GRASS_SKY,
    CAVE_GROUND,
    CAVE_WALL,
    MANSION_GROUND,
    MANSION_WALL,

    // Reserve other texture units here...

    COUNT // Keep last
};

}
