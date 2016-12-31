#pragma once
#include <dm/dm.hpp>

namespace dm {

// ENEMY0 is not "the first enemy", it is the first of 
// possible types an enemy can be.
// Names which end by a number might (should) be
// renamed to something more explicit later.
// Eg. "KEY_BLUE" instead of "KEY2".
enum class Tile {
    GROUND  = 0xFFFFFF,
    WALL    = 0x000000,
    DOOR    = 0xAA7722,
    WATER   = 0x0000FF,
    START   = 0xFF0000,
    EXIT    = 0x00FF00,
    CAGE0   = 0x0FF0FF, 
    CAGE1   = 0x0FE0FF,
    CAGE2   = 0x0FD0FF,
    CAGE3   = 0x0FC0FF,
    CAGE4   = 0x0FB0FF,
    CAGE5   = 0x0FA0FF,
    KEY0    = 0x0FF00F,
    KEY2    = 0x0FE00F,
    KEY1    = 0x0FD00F,
    KEY3    = 0x0FC00F,
    KEY4    = 0x0FB00F,
    KEY5    = 0x0FA00F,
    ENEMY0  = 0xFF00FF,
    ENEMY1  = 0xFF00EF,
    ENEMY2  = 0xFF00DF,
    ENEMY3  = 0xFF00CF,
    ENEMY4  = 0xFF00BF,
    ENEMY5  = 0xFF00AF,
    FRIEND0 = 0xFFFF0F,
    FRIEND1 = 0xFFFF0E,
    FRIEND2 = 0xFFFF0D,
    FRIEND3 = 0xFFFF0C,
    FRIEND4 = 0xFFFF0B,
    FRIEND5 = 0xFFFF0A,

    // New members should go here.

    COUNT // Keep last
};

struct TileSet : public PPMImage {
    glm::ivec2 spawn_pos;
    float spawn_angle_y;

    static bool isTileCage(Tile tile);
    static bool isTileKey(Tile tile);
    static bool isTileEnemy(Tile tile);
    static bool isTileFriend(Tile tile);
    static bool isTileWalkable(Tile tile);
    bool isValid() const;
    void recomputeInfo();
    static Tile rgb24ToTile(rgb24 rgb);
    Tile getTileAt(size_t x, size_t y) const;
};


typedef signed int LifeValue; 
// Not unsigned, to prevent accidental subtraction overflows.


struct TileObject {
    Lerp<glm::vec3> position;
    Lerp<float>     angle_y; // in radians
    void face(const TileObject &other);
    glm::mat4 getModelMatrix() const;
};

struct QuadEntity {
    size_t quad_index;
};


struct Character {
    LifeValue       life;
    LifeValue       life_max;
    float           speed; // Tiles per second.
    float           angular_speed; // 90° rotations per second.
};

struct Friend : public Character, public QuadEntity, public TileObject {
    size_t quad_index; // index to the trans_quad_batch.
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

struct Hero : public Fighter, public TileObject {
    Keyring keyring;
    Attack close_range_attack;
    Attack long_range_attack;
    bool is_long_range_attack_unlocked;
};

struct Enemy : public Fighter, public QuadEntity, public TileObject {
    // void detectHeroAndMove(const Hero &hero);
};

struct Enemy0 : public Enemy { /*Enemy0(); ~Enemy0(); */};
struct Enemy1 : public Enemy { /*Enemy1(); ~Enemy1(); */};
struct Enemy2 : public Enemy { /*Enemy2(); ~Enemy2(); */};
struct Enemy3 : public Enemy { /*Enemy3(); ~Enemy3(); */};
struct Enemy4 : public Enemy { /*Enemy4(); ~Enemy4(); */};
struct Enemy5 : public Enemy { /*Enemy5(); ~Enemy5(); */};

struct Key  : public QuadEntity, public TileObject {};
struct Cage : public QuadEntity, public TileObject {};
struct Door : public TileObject {};


struct Dungeon {
    PerspectiveView    view;
    static Lerp<float> fade_transition;
    GLQuadBatch        walls_quad_batch;
    GLQuadBatch        ground_quad_batch;
    GLQuadBatch        ceiling_quad_batch;
    GLQuadBatch        doors_quad_batch;
    GLQuadBatch        trans_quad_batch; // enemies, friends, cages, keys, exit
    // TODO cages, doors, billboards, stairs
    // TODO water ?
    // TODO HUD ? Map on HUD ?
    // TODO Dialogues ?
    bool               should_render_ceiling;

    size_t               floor_index;
    Hero                 hero;
    TileSet              tiles;
    std::vector<Enemy>   enemies;
    std::vector<Friend>  friends;
    std::vector<Key>     keys;
    std::vector<Cage>    cages;
    std::vector<Door>    doors;

     Dungeon(glm::ivec2 viewport_size);
    ~Dungeon();

    void prepare(size_t i);
    void reshape(glm::ivec2 new_viewport_size);
    GameplayType nextFrame(const Input &input, uint32_t fps);
    void renderGL() const;

private:
    static size_t refcount;
    static void setupGL();
    static void cleanupGL();
    static GLuint tex_grass_ground  ;
    static GLuint tex_grass_wall    ;
    static GLuint tex_grass_sky     ;
    static GLuint tex_cave_ground   ;
    static GLuint tex_cave_wall     ;
    static GLuint tex_mansion_ground;
    static GLuint tex_mansion_wall  ;
    void fillFloorDataFromTileSet();
};



} // namespace dm
