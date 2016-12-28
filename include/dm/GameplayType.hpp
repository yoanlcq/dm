#pragma once

namespace dm {

// For lack of a better name...
// We're either in the welcome screen, or on the world map, or in a dungeon.
// Concerns differ while updating, rendering, and stuff.
enum class GameplayType {
    WELCOME_SCREEN = 0, // Leave it to 0
    WORLD_MAP,
    DUNGEON
};

}
