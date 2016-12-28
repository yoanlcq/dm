#pragma once
#include <dm/dm.hpp>

namespace dm {

struct DungeonStyleGL {
    glm::vec3 clearcolor;
    GLuint tex_wall;
    GLuint tex_ground;
    GLuint tex_ceiling;
};


// ENEMY0 is not "the first enemy", it is the first of 
// possible types an enemy can be.
// Names which end by a number might (should) be
// renamed to something more explicit later.
// Eg. "KEY_BLUE" instead of "KEY2".
enum Tile {
    GROUND  = 0xFFFFFF,
    WALL    = 0x000000,
    DOOR    = 0xAA7722,
    WATER   = 0x0000FF,
    START   = 0xFF0000,
    EXIT    = 0x0000FF,
    CAGE0   = 0x0FF0FF, 
    CAGE1   = 0x0FE0FF,
    CAGE2   = 0x0FD0FF,
    CAGE3   = 0x0FC0FF,
    CAGE4   = 0x0FB0FF,
    CAGE5   = 0x0FA0FF,
    KEY0    = 0x0FF0F0,
    KEY2    = 0x0FE0F0,
    KEY1    = 0x0FD0F0,
    KEY3    = 0x0FC0F0,
    KEY4    = 0x0FB0F0,
    KEY5    = 0x0FA0F0,
    ENEMY0  = 0xFF000F,
    ENEMY1  = 0xFF000E,
    ENEMY2  = 0xFF000D,
    ENEMY3  = 0xFF000C,
    ENEMY4  = 0xFF000B,
    ENEMY5  = 0xFF000A,
    FRIEND0 = 0xFFFF0F,
    FRIEND1 = 0xFFFF0E,
    FRIEND2 = 0xFFFF0D,
    FRIEND3 = 0xFFFF0C,
    FRIEND4 = 0xFFFF0B,
    FRIEND5 = 0xFFFF0A,

    // New members should go here.

    COUNT // Keep last
};

typedef Image<Tile> TileSet;

typedef signed int LifeValue; 
// Not unsigned, to prevent accidental subtraction overflows.


struct Character {
    LifeValue       life;
    LifeValue       life_max;
    Lerp<glm::vec3> position;
    Lerp<glm::vec2> direction; // Which way it is facing.
    float           speed; // How fast it moves.
    float           angular_speed; // How fast it turns around.
};

struct Friend : public Character {
    std::vector<std::string> dialogue_lines;
};

struct Attack {
    typedef bool (*HitChecker)(glm::ivec2 target_pos);
    // 'target_pos' must be relative to the attack's user.

    LifeValue  damage;
    Lerp<bool> started, completed;
    float      start_speed, after_speed;

    // void use();
    // static const Attack SIMPLE;
    // static const Attack CLOSE_RANGE;
    // static const Attack LONG_RANGE;
};


struct Fighter : public Character {
    float damage_multiplier;
    Attack basic_attack;
};

// A key type, and the number of those keys.
typedef std::map<Tile, size_t> Keyring;

struct Hero : public Fighter {
    Keyring keyring;
    Attack close_range_attack;
    Attack long_range_attack;
    bool is_long_range_attack_unlocked;
};

struct Enemy : public Fighter {
    // void detectHeroAndMove(const Hero &hero);
};

struct Enemy0 : public Enemy { /*Enemy0(); ~Enemy0(); */};
struct Enemy1 : public Enemy { /*Enemy1(); ~Enemy1(); */};
struct Enemy2 : public Enemy { /*Enemy2(); ~Enemy2(); */};
struct Enemy3 : public Enemy { /*Enemy3(); ~Enemy3(); */};
struct Enemy4 : public Enemy { /*Enemy4(); ~Enemy4(); */};
struct Enemy5 : public Enemy { /*Enemy5(); ~Enemy5(); */};

enum FloorStyle {
    OUTSIDE_GRASS,
    INSIDE_ROCK
};

struct Floor {
    FloorStyle style;
    TileSet tiles;
    std::vector<Enemy> enemies;
    std::vector<Friend> friends;
    Hero hero;
};


typedef size_t DungeonIndex;
struct Dungeon {
    static Lerp<float> fade_transition;
    std::vector<Floor> all_floors;
    Floor*             floor;
    PerspectiveView    view;

     Dungeon(glm::ivec2 viewport_size);
    ~Dungeon();

    void reshape(glm::ivec2 new_viewport_size);
    GameplayType nextFrame(const Input &input, uint32_t fps);
    void renderGL() const;

private:
    static size_t refcount;
    static void setupGL();
    static void cleanupGL();
};



} // namespace dm
