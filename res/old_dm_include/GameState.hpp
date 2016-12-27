#pragma once
#include <dm/dm.hpp>

namespace dm {

// On a besoin de distinguer l'état "logique" du jeu de son état "visuel".
// L'état visuel a besoin d'etre mis à jour aussi vite que l'écran se rafraichit,
// et pas tous les écrans ne sont à 60Hz.
//
// On a besoin que le gameplay soit consistant sur tous les appareils, donc
// la fréquence de mise à jour de l'état "logique" devrait ^etre fixée et connue.
//
// Ensuite, l'état visuel est le rendu d'une interpolation entre l'état logique
// actuel et le suivant.
//
//
// Ici, un Donjon est défini comme :
// - Un ensemble d'étages (floors).
// - A tout moment, le héros est dans l'étage donné d'un donjon donné.
// On peut avoir plusieurs donjons.

// Types de cases.
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
    // ^ C'est le délai, en millisecondes, entre deux mises à 
    // jour de l'état logique.
    uint64_t tick_count;
    // ^ Cette valeur est incrémentée à chaque MAJ.
    uint32_t time_ms, prev_time_ms;
    void update(Input &input);
    Character hero;
    Dungeon all_dungeons;
    Dungeon *current_dungeon;
};

}
