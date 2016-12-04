#pragma once
#include <dm/dm.hpp>

namespace dm {

// We need to distinguish game state from visual state.
// Basically :
// - visual state update frequency depends on the framerate.
// - game state update frequency is fixed, and is the same on all machines.
// This way, we can ensure consistent gameplay experience across various
// monitors while still making best use of their refresh rate.


// The GameState struct contains info about the hero's position, orientation,
// the monsters's positions, etc.
// It might contain the previous positions so the VisualState can interpolate
// them at the time of rendering.

// Here, we treat dungeons as set of floors.
// - Any dungeon has any number of floors.
// - At any one time, the hero is at a given floor at a given dungeon.
// - We may have various dungeons.

enum Tile {
    TILE_WALL    = 0x000000,
    TILE_EMPTY   = 0xFFFFFF,
    TILE_DOOR    = 0xAA7722,
    TILE_WATER   = 0x0000FF,
    TILE_ENTRY   = 0xFF0000,
    TILE_EXIT    = 0x00FF00,
    TILE_CHEST   = 0xFFFF00,
    TILE_WEAPON  = 0x00FFFF,
    TILE_MONSTER = 0xcccc00
    // Perhaps more stuff later...
};

struct Character {
    glm::ivec2 position, moving_direction;
    uint32_t moving_transition, prev_moving_transition;
    static const uint32_t MOVING_TRANSITION_STEP;
    int32_t angle_deg, angle_direction, prev_angle_deg; // degrees
    uint32_t health, mana, atk, def; // Why not ? x)
    bool is_moving, is_attacking, is_rotating;
    Character();
    ~Character();
    void move(glm::ivec2 dir);
    void rotate(int32_t dir);
    void attack();
    void update();
};

struct Floor {
    std::vector<Character> enemies;
    Tile **tiles;
    glm::ivec2 tile_count;
};

struct Dungeon {
    std::vector<Floor> floors;
    Floor *current_floor;
};

struct GameState {
    uint32_t tick_delay_ms; 
    // ^ This is the delay, in milliseconds, between game state updates.
    // Different from visual state updates, which are per-frame.
    uint64_t tick_count;
    uint32_t time_ms, prev_time_ms;
    void update(Input &input);
    Character hero;
    Dungeon all_dungeons;
    Dungeon *current_dungeon;
};

}
