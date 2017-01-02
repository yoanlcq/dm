#include <dm/dm.hpp>

using namespace std;
using namespace glm;

namespace dm {


static float angleClosestQuarter(float angle) {
    angle = fmod(angle, M_PI*2);
    if(angle < 0)
        angle += M_PI*2;
    return 2.f*M_PI*roundf(angle/(M_PI/2.f))/4.f;
}


Tile TileSet::rgb24ToTile(rgb24 col) {
    uint32_t tile = (col.r<<16) | (col.g<<8) | col.b;
    return (Tile)tile;
}
rgb24 TileSet::tileToRgb24(Tile tile) {
    uint32_t col = (uint32_t)tile;
    return rgb24((col>>16)&0xff, (col>>8)&0xff, col&0xff);
}


bool TileSet::isValid() const {
#define HELPER(cst) \
    if(tile == Tile::cst) \
        continue;
    bool is_valid = true;
    size_t tile_start_count(0);
    size_t tile_exit_count(0);
    ivec2 s_spawn_pos;
    for(size_t y=0 ; y<h ; ++y) for(size_t x=0 ; x<w ; ++x) {
        rgb24 tilecol = (*this)[y][x];
        Tile tile = getTileAt(x,y);

        if(tile == Tile::START) {
            ++tile_start_count;
            if(tile_start_count>1) {
                cerr << "Warning : Conflicting spawn tiles at "
                     << s_spawn_pos << " and " << ivec2(x,y) 
                     << "!" << endl;
                is_valid = false;
            }
            s_spawn_pos = ivec2(x,y);
        }
        if(tile == Tile::EXIT)
            ++tile_exit_count;

        HELPER(GROUND );
        HELPER(WALL   );
        HELPER(DOOR   );
        HELPER(WATER  );
        HELPER(START  );
        HELPER(EXIT   );
        HELPER(CAGE0  ); 
        HELPER(CAGE1  );
        HELPER(CAGE2  );
        HELPER(CAGE3  );
        HELPER(CAGE4  );
        HELPER(CAGE5  );
        HELPER(KEY0   );
        HELPER(KEY2   );
        HELPER(KEY1   );
        HELPER(KEY3   );
        HELPER(KEY4   );
        HELPER(KEY5   );
        HELPER(ENEMY0 );
        HELPER(ENEMY1 );
        HELPER(ENEMY2 );
        HELPER(ENEMY3 );
        HELPER(ENEMY4 );
        HELPER(ENEMY5 );
        HELPER(FRIEND0);
        HELPER(FRIEND1);
        HELPER(FRIEND2);
        HELPER(FRIEND3);
        HELPER(FRIEND4);
        HELPER(FRIEND5);

        cerr << "Warning : the tile at (" << x << ","
              << y << ")'s color is unrecognized (value:"
              << "0x"
              << setfill('0') << setw(2) << hex << unsigned(tilecol.r)
              << setfill('0') << setw(2) << hex << unsigned(tilecol.g)
              << setfill('0') << setw(2) << hex << unsigned(tilecol.b)
              << ")" << endl;
        is_valid = false;
    }

    if(!tile_start_count) {
        cerr << "Warning : there's no spawn tile!" << endl;    
        is_valid = false;
    }
    if(!tile_exit_count) {
        cerr << "Warning : there's no exit tile!" << endl;    
        is_valid = false;
    }

    return is_valid;
#undef HELPER
}

void TileSet::recomputeInfo() {
    assert(isValid());
    for(size_t y=0 ; y<h ; ++y) for(size_t x=0 ; x<w ; ++x) {
        Tile tile = getTileAt(x,y);
        if(tile == Tile::START) {
            spawn_pos = ivec2(x,y);

            if(x+1 <  w) if(getTileAt(x+1, y) != Tile::WALL) spawn_angle_y = 0;
            if(x   >  0) if(getTileAt(x-1, y) != Tile::WALL) spawn_angle_y = M_PI;
            if(y+1 <  h) if(getTileAt(x, y+1) != Tile::WALL) spawn_angle_y = 3*M_PI/2;
            if(y   >  0) if(getTileAt(x, y-1) != Tile::WALL) spawn_angle_y = M_PI/2;

            return;
        }
    }
}

Tile TileSet::getTileAt(size_t x, size_t y) const {
    return rgb24ToTile((*this)[y][x]);
}
void TileSet::setTileAt(size_t x, size_t y, Tile tile) {
    (*this)[y][x] = tileToRgb24(tile);
}

void TileSet::swapTilesAt(size_t x1, size_t y1, size_t x2, size_t y2) {
    std::swap((*this)[y1][x1], (*this)[y2][x2]);
}

bool TileSet::isTileCage(Tile tile) {
    return tile == Tile::CAGE0
        || tile == Tile::CAGE1
        || tile == Tile::CAGE2
        || tile == Tile::CAGE3
        || tile == Tile::CAGE4
        || tile == Tile::CAGE5;
}
bool TileSet::isTileKey(Tile tile) {
    return tile == Tile::KEY0
        || tile == Tile::KEY1
        || tile == Tile::KEY2
        || tile == Tile::KEY3
        || tile == Tile::KEY4
        || tile == Tile::KEY5;
}
bool TileSet::isTileEnemy(Tile tile) {
    return tile == Tile::ENEMY0
        || tile == Tile::ENEMY1
        || tile == Tile::ENEMY2
        || tile == Tile::ENEMY3
        || tile == Tile::ENEMY4
        || tile == Tile::ENEMY5;

}
bool TileSet::isTileFriend(Tile tile) {
    return tile == Tile::FRIEND0
        || tile == Tile::FRIEND1
        || tile == Tile::FRIEND2
        || tile == Tile::FRIEND3
        || tile == Tile::FRIEND4
        || tile == Tile::FRIEND5;
}

bool TileSet::isTileWalkable(Tile tile) {
    return !(tile==Tile::WALL  || tile==Tile::DOOR
          || isTileCage(tile)  || isTileEnemy(tile) || isTileFriend(tile));
}
Tile TileSet::cageToKey(Tile tile) {
    assert(isTileCage(tile));
    switch(tile) {
    default:          return Tile::KEY0;
    case Tile::CAGE0: return Tile::KEY0;
    case Tile::CAGE1: return Tile::KEY1;
    case Tile::CAGE2: return Tile::KEY2;
    case Tile::CAGE3: return Tile::KEY3;
    case Tile::CAGE4: return Tile::KEY4;
    case Tile::CAGE5: return Tile::KEY5;
    };
}

int TileSet::cageTileToIndex(Tile tile) {
    assert(isTileCage(tile));
    switch(tile) {
    default:          return -1;
    case Tile::CAGE0: return 0;
    case Tile::CAGE1: return 1;
    case Tile::CAGE2: return 2;
    case Tile::CAGE3: return 3;
    case Tile::CAGE4: return 4;
    case Tile::CAGE5: return 5;
    };
}

int TileSet::keyTileToIndex(Tile tile) {
    assert(isTileKey(tile));
    switch(tile) {
    default:          return -1;
    case Tile::KEY0: return 0;
    case Tile::KEY1: return 1;
    case Tile::KEY2: return 2;
    case Tile::KEY3: return 3;
    case Tile::KEY4: return 4;
    case Tile::KEY5: return 5;
    };
}

int TileSet::enemyTileToIndex(Tile tile) {
    assert(isTileEnemy(tile));
    switch(tile) {
    default:          return -1;
    case Tile::ENEMY0: return 0;
    case Tile::ENEMY1: return 1;
    case Tile::ENEMY2: return 2;
    case Tile::ENEMY3: return 3;
    case Tile::ENEMY4: return 4;
    case Tile::ENEMY5: return 5;
    };
}

int TileSet::friendTileToIndex(Tile tile) {
    assert(isTileFriend(tile));
    switch(tile) {
    default:          return -1;
    case Tile::FRIEND0: return 0;
    case Tile::FRIEND1: return 1;
    case Tile::FRIEND2: return 2;
    case Tile::FRIEND3: return 3;
    case Tile::FRIEND4: return 4;
    case Tile::FRIEND5: return 5;
    };
}





TileObject::TileObject() 
    : position(vec3(0,0,0)), 
      angle_y(float(M_PI)/2), 
      m_scale(1,1,1),
      was_tile_swapped(false) {}


ivec2 TileObject::getNextTilePos() const {
    return ivec2(
        roundf(position.next.x), 
        roundf(position.next.z)
    );
}
ivec2 TileObject::getTilePos() const {
    return ivec2(
        roundf(position.getCurrent().x), 
        roundf(position.getCurrent().z)
    );
}


static bool forwardHitChecker(ivec2 org, float org_angle_y, ivec2 target, TileSet &tiles) {
    org_angle_y = angleClosestQuarter(org_angle_y);
    vec3 dir = rotate(mat3(), org_angle_y) * vec3(1, 0, 0);
    dir.y = -dir.y;
    ivec2 hit_pos(org.x+roundf(dir.x), org.y+roundf(dir.y));
    return hit_pos == target;
}

Attack::Attack() 
    : check_hit(forwardHitChecker),
      damage(5), 
      launch_progress(0), 
      post_progress(0),
      launch_speed(1/.5f),
      post_speed(1/.6f)
    {}

Fighter::Fighter() : damage_multiplier(1) {}


size_t Dungeon::refcount(0);
GLuint Dungeon::tex_grass_ground  (0);
GLuint Dungeon::tex_grass_wall    (0);
GLuint Dungeon::tex_grass_sky     (0);
GLuint Dungeon::tex_cave_ground   (0);
GLuint Dungeon::tex_cave_wall     (0);
GLuint Dungeon::tex_mansion_ground(0);
GLuint Dungeon::tex_mansion_wall  (0);
GLuint Dungeon::tex_trans_atlas   (0);
GLuint Dungeon::tex_door          (0);

void Dungeon::setupGL() {
    hope(tex_grass_ground   = GLTexture_fromFile("res/tex_sol_prairie.jpg"));
    hope(tex_grass_wall     = GLTexture_fromFile("res/tex_mur_prairie.png"));
    hope(tex_grass_sky      = GLTexture_fromFile("res/tex_ciel_prairie.png"));
    hope(tex_cave_ground    = GLTexture_fromFile("res/tex_sol_grotte.jpg"));
    hope(tex_cave_wall      = GLTexture_fromFile("res/tex_mur_grotte.jpg"));
    hope(tex_mansion_ground = GLTexture_fromFile("res/tex_sol_manoir.jpg"));
    hope(tex_mansion_wall   = GLTexture_fromFile("res/tex_mur_manoir.jpg"));
    hope(tex_trans_atlas    = GLTexture_fromFile("res/trans.png"));
    hope(tex_door           = GLTexture_fromFile("res/door.jpg"));

    GLTexture_bindToUnit(tex_grass_ground  , TextureUnit::GRASS_GROUND);
    GLTexture_bindToUnit(tex_grass_wall    , TextureUnit::GRASS_WALL);
    GLTexture_bindToUnit(tex_grass_sky     , TextureUnit::GRASS_SKY);
    GLTexture_bindToUnit(tex_cave_ground   , TextureUnit::CAVE_GROUND);
    GLTexture_bindToUnit(tex_cave_wall     , TextureUnit::CAVE_WALL);
    GLTexture_bindToUnit(tex_mansion_ground, TextureUnit::MANSION_GROUND);
    GLTexture_bindToUnit(tex_mansion_wall  , TextureUnit::MANSION_WALL);
    GLTexture_bindToUnit(tex_trans_atlas   , TextureUnit::TRANS_ATLAS);
    GLTexture_bindToUnit(tex_door          , TextureUnit::DOOR);
}
void Dungeon::cleanupGL() {
    glDeleteTextures(1, &tex_grass_ground  );
    glDeleteTextures(1, &tex_grass_wall    );
    glDeleteTextures(1, &tex_grass_sky     );
    glDeleteTextures(1, &tex_cave_ground   );
    glDeleteTextures(1, &tex_cave_wall     );
    glDeleteTextures(1, &tex_mansion_ground);
    glDeleteTextures(1, &tex_mansion_wall  );
    glDeleteTextures(1, &tex_trans_atlas   );
    glDeleteTextures(1, &tex_door          );
}

Dungeon:: Dungeon(ivec2 viewport_size) 
    : view(viewport_size),
      hud_view(viewport_size)
{
    if(!refcount)
        setupGL();
    ++refcount;

    hud_life_quad_batch.instances.resize(2);
    hud_fader_quad_batch.instances.resize(1);
    hud_fader_quad_batch.rgba_fx = vec4(0,0,0,0);
    fader_txt.lines.resize(2);
}
Dungeon::~Dungeon() {
    --refcount;
    if(!refcount)
        cleanupGL();
}


void Dungeon::reshape(ivec2 new_viewport_size) {
    
    view.reshape(new_viewport_size);
    hud_view.reshape(new_viewport_size);

    const float dialogue_line_w = dialogue.getStringWidth("Oh no! the princess was kidnapped!");
    dialogue.position = vec3(
        -dialogue_line_w/2.f,
        -hud_view.getHalfHeight() + 2.f*dialogue.line_height,
        0
    );
    dialogue.updateQuadBatch();

    if(dialogue_box_quad_batch.instances.size()) {
        dialogue_box_quad_batch.instances[0].modelmatrix 
            = translate(mat4(), vec3(0, 2.f*dialogue.line_height-hud_view.getHalfHeight(), 0))
            * scale(vec3(1.1f, 1.8f, 1))
            * scale(vec3(dialogue_line_w, 2.f*dialogue.line_height, 1));
        dialogue_box_quad_batch.updateInstancesVBO();
    }


    hud_keys_quad_batch.instances.clear();
    hud_keys_quad_batch.texture_unit = TextureUnit::TRANS_ATLAS;
    for(size_t i=0 ; i<hero.keys.size() ; ++i) {
        const float size = .14f;
        GLQuadBatch::QuadInstance quad;
        quad.modelmatrix = translate(mat4(), vec3(hud_view.half_width-size/2-size*i, hud_view.getHalfHeight()-size/2, 0))
                         * scale(vec3(size, size, 1));
        quad.sprite_pos  = vec2(512*TileSet::keyTileToIndex(hero.keys[i]), 4096-1024)/4096.f;
        quad.sprite_size = vec2(512, 512)/4096.f;
        hud_keys_quad_batch.instances.push_back(quad);
    }
    hud_keys_quad_batch.updateInstancesVBO();



    assert(hud_life_quad_batch.instances.size()==2);
    const float lifebar_1_w = .004f;
    const float lifebar_w = hero.life_max * lifebar_1_w;
    const float lifebar_h = .14f;


   hud_life_quad_batch.instances[1].sprite_pos  = vec2(4096-512,4096-512)/4096.f;
   hud_life_quad_batch.instances[0].sprite_pos  = vec2(4096-512,4096-1024)/4096.f;
   hud_life_quad_batch.instances[0].sprite_size = vec2(512, 512)/4096.f;
   hud_life_quad_batch.instances[1].sprite_size = vec2(512, 512)/4096.f;

    // Too lazy to refactor these two
    hud_life_quad_batch.instances[0].modelmatrix
        = translate(mat4(), vec3(-hud_view.half_width+lifebar_w/2+.05f, hud_view.getHalfHeight()-lifebar_h/2-.025f, 0))
        * scale(vec3(lifebar_1_w*hero.life_max, lifebar_h, 1));

    hud_life_quad_batch.instances[1].modelmatrix
        = translate(mat4(), vec3(-hud_view.half_width+lifebar_1_w*hero.life/2+.05f, hud_view.getHalfHeight()-lifebar_h/2-.025f, 0))
        * scale(vec3(lifebar_1_w*hero.life, lifebar_h, 1));
    
    hud_life_quad_batch.texture_unit = TextureUnit::TRANS_ATLAS;
    hud_life_quad_batch.updateInstancesVBO();


    assert(hud_fader_quad_batch.instances.size());
    hud_fader_quad_batch.texture_unit = TextureUnit::BLACK_FADER;
    hud_fader_quad_batch.instances[0].modelmatrix 
        = scale(vec3(2*hud_view.half_width, 2*hud_view.getHalfHeight(), 1));
    hud_fader_quad_batch.updateInstancesVBO();

    assert(fader_txt.lines.size() == 2);
    fader_txt.line_height = hud_view.getHalfHeight()/6.f;
    fader_txt.position.x = -fader_txt.getLineWidth(0)/2;
    fader_txt.updateQuadBatch();
}


void TileObject::face(const TileObject &other) {
    // Kinda hard to choose betwwen the two.
#if 1 
    vec3 facing_vec = position.getCurrent() - other.position.getCurrent();
    angle_y = atan2f(-facing_vec.z, facing_vec.x)+M_PI;
#else
    angle_y = other.angle_y + M_PI;
#endif
}

mat4 TileObject::getModelMatrix() const {
    return translate(mat4(), position.getCurrent())
         * translate(mat4(), vec3(0, -(1-m_scale.y)/2.f, 0))
         * rotate(angle_y+float(M_PI)/2.f, vec3(0,1,0))
         * scale(m_scale);
}


mat4 Cage::getQuadModelMatrix(size_t i) {
    return translate(mat4(), position.getCurrent())
         * rotate(quad_angles_y[i]-float(M_PI)/2.f, vec3(0,1,0))
         * translate(mat4(), vec3(0,0,-.45f))
         * scale(vec3(.90f, 1, 1));
}


const float Door::OPENING_SPEED = .1f;

static const float DOOR_EPSILON = .001f;
static const float DOOR_HALFW = .5f-DOOR_EPSILON;

mat4 Door::getLModelMatrix() const {
    return translate(mat4(), position.getCurrent())
         * rotate(angle_y-float(M_PI)/2.f, vec3(0,1,0))
         * translate(mat4(), vec3(-DOOR_HALFW, 0, 0))
         * rotate(opening_angle.getCurrent(), vec3(0,1,0))
         * translate(mat4(), vec3(DOOR_HALFW/2, 0, 0))
         * scale(vec3(DOOR_HALFW, 1, 1));
}

mat4 Door::getRModelMatrix() const {
    return translate(mat4(), position.getCurrent())
         * rotate(angle_y-float(M_PI)/2.f, vec3(0,1,0))
         * translate(mat4(), vec3(DOOR_HALFW, 0, 0))
         * rotate(-opening_angle.getCurrent(), vec3(0,1,0))
         * translate(mat4(), vec3(-DOOR_HALFW/2, 0, 0))
         * scale(vec3(DOOR_HALFW, 1, 1));
}


static const vec2 ENEMY_GLADE_IDLE_SPRPOS     = vec2(0000, 4096-2104)/4096.f;
static const vec2 ENEMY_GLADE_ATTACK_SPRPOS   = vec2(1024, 4096-2104)/4096.f;
static const vec2 ENEMY_MANSION_IDLE_SPRPOS   = vec2(2048, 4096-2104)/4096.f;
static const vec2 ENEMY_MANSION_ATTACK_SPRPOS = vec2(3072, 4096-2104)/4096.f;
static const vec2 ENEMY_CAVE_IDLE_SPRPOS      = vec2(0000, 4096-3222)/4096.f;
static const vec2 ENEMY_CAVE_ATTACK_SPRPOS    = vec2(1024, 4096-3222)/4096.f;
static const vec2 ENEMY_BOSS_IDLE_SPRPOS      = vec2(2048, 4096-3222)/4096.f;
static const vec2 ENEMY_BOSS_ATTACK_SPRPOS    = vec2(3072, 4096-3222)/4096.f;
static const vec2 ENEMY_SPRSIZE = vec2(1024.f,1024.f)/4096.f;


void Dungeon::fillFloorDataFromTileSet() {

    enemies.clear();
    friends.clear();
    keys.clear();
    cages.clear();
    doors.clear();

    for(size_t y=0 ; y<tiles.h ; ++y) for(size_t x=0 ; x<tiles.w ; ++x) {

        Tile tile = tiles.getTileAt(x,y);
        
        GLQuadBatch::QuadInstance quad;

#define ADD_TRANS_QUAD(objvec) { \
             (objvec).push_back({}); \
            ((objvec).end()-1)->quad_index = trans_quad_batch.instances.size(); \
            ((objvec).end()-1)->position = vec3(x, 0, y); \
            ((objvec).end()-1)->face(hero); \
            quad.modelmatrix = ((objvec).end()-1)->getModelMatrix(); \
            trans_quad_batch.instances.push_back(quad); \
        }
#define ADD_WALL_QUAD(qbatch, quad_angle_y) { \
            quad.modelmatrix = translate(mat4(), vec3(x,0,y)) \
                             * rotate(float(quad_angle_y)-float(M_PI)/2.f, vec3(0,1,0)) \
                             * translate(mat4(), vec3(0,0,-.5f)); \
            quad.sprite_size = vec2(.999,.999); \
            if(tile == Tile::EXIT) { \
                quad.sprite_pos  = vec2(4096-1024+10,4096-512)/4096.f; \
                quad.sprite_size = vec2(512-10,512)/4096.f; \
            } \
            (qbatch).instances.push_back(quad); \
            quad.sprite_size = vec2(0,0); \
            quad.sprite_size = vec2(1,1); \
        }
#define ADD_CAGE_QUAD(qbatch, quad_angle_y) { \
            (cages.end()-1)->quad_indices.push_back((qbatch).instances.size()); \
            (cages.end()-1)->quad_angles_y.push_back(quad_angle_y); \
            (cages.end()-1)->position = vec3(x, 0, y); \
            quad.modelmatrix = (cages.end()-1)->getQuadModelMatrix((cages.end()-1)->quad_angles_y.size()-1); \
            quad.sprite_pos  = vec2(512*TileSet::cageTileToIndex(tile),4096-512)/4096.f; \
            quad.sprite_size = vec2(512,512)/4096.f; \
            (qbatch).instances.push_back(quad); \
        }
#define ADD_WALL_QUADS(qbatch) { \
            if(x+1 < tiles.w) if(tiles.getTileAt(x+1, y) != Tile::WALL) ADD_WALL_QUAD((qbatch), 0.f); \
            if(x > 0)         if(tiles.getTileAt(x-1, y) != Tile::WALL) ADD_WALL_QUAD((qbatch), M_PI); \
            if(y+1 < tiles.h) if(tiles.getTileAt(x, y+1) != Tile::WALL) ADD_WALL_QUAD((qbatch), 3*M_PI/2); \
            if(y > 0)         if(tiles.getTileAt(x, y-1) != Tile::WALL) ADD_WALL_QUAD((qbatch), M_PI/2); \
        }
#define ADD_CAGE_QUADS(qbatch) { \
            bool should_pop = true; \
            cages.push_back({}); \
            if(x+1 < tiles.w) if(tiles.getTileAt(x+1, y) != Tile::WALL) { should_pop=false; ADD_CAGE_QUAD((qbatch), 0.f); } \
            if(x > 0)         if(tiles.getTileAt(x-1, y) != Tile::WALL) { should_pop=false; ADD_CAGE_QUAD((qbatch), M_PI); } \
            if(y+1 < tiles.h) if(tiles.getTileAt(x, y+1) != Tile::WALL) { should_pop=false; ADD_CAGE_QUAD((qbatch), 3*M_PI/2); } \
            if(y > 0)         if(tiles.getTileAt(x, y-1) != Tile::WALL) { should_pop=false; ADD_CAGE_QUAD((qbatch), M_PI/2); } \
            if(should_pop) \
                cages.erase(cages.end()-1);    \
        }
#define ADD_DOOR_QUAD(qbatch, quad_angle_y) { \
            doors.push_back({}); \
            (doors.end()-1)->opening_angle.reset(0); \
            (doors.end()-1)->opening_angle.next = M_PI/2; \
            (doors.end()-1)->position.reset(vec3(x,0,y)); \
            (doors.end()-1)->angle_y.reset(quad_angle_y); \
            (doors.end()-1)->l_quad_index = (qbatch).instances.size(); \
            quad.modelmatrix = (doors.end()-1)->getLModelMatrix(); \
            (qbatch).instances.push_back(quad); \
            (doors.end()-1)->r_quad_index = (qbatch).instances.size(); \
            quad.modelmatrix = (doors.end()-1)->getRModelMatrix(); \
            (qbatch).instances.push_back(quad); \
        }
#define ADD_DOOR_QUADS(qbatch) { \
             if(x+1 < tiles.w && tiles.getTileAt(x+1, y) != Tile::WALL) { ADD_DOOR_QUAD((qbatch), 0.f); } \
        else if(x > 0         && tiles.getTileAt(x-1, y) != Tile::WALL) { ADD_DOOR_QUAD((qbatch), M_PI); } \
        else if(y+1 < tiles.h && tiles.getTileAt(x, y+1) != Tile::WALL) { ADD_DOOR_QUAD((qbatch), 3*M_PI/2); } \
        else if(y > 0         && tiles.getTileAt(x, y-1) != Tile::WALL) { ADD_DOOR_QUAD((qbatch), M_PI/2); } \
        }

        if(TileSet::isTileEnemy(tile)) {
            ADD_TRANS_QUAD(enemies);
            (enemies.end()-1)->speed = 1;
            (enemies.end()-1)->feel_radius = 8;
            (enemies.end()-1)->life_max = 20;
            (enemies.end()-1)->life = (enemies.end()-1)->life_max;
            auto quad = (trans_quad_batch.instances.end()-1);
            switch(dungeon_index) {
            case 0: case 1:
                (enemies.end()-1)->idle_spr_pos   = ENEMY_GLADE_IDLE_SPRPOS;
                (enemies.end()-1)->attack_spr_pos = ENEMY_GLADE_ATTACK_SPRPOS;
                break;
            case 2:
                (enemies.end()-1)->idle_spr_pos   = ENEMY_CAVE_IDLE_SPRPOS;
                (enemies.end()-1)->attack_spr_pos = ENEMY_CAVE_ATTACK_SPRPOS;
                break;
            case 3: case 4:
                (enemies.end()-1)->idle_spr_pos   = ENEMY_MANSION_IDLE_SPRPOS;
                (enemies.end()-1)->attack_spr_pos = ENEMY_MANSION_ATTACK_SPRPOS;
                break;
            }
            quad->sprite_pos  = (enemies.end()-1)->idle_spr_pos;
            quad->sprite_size = ENEMY_SPRSIZE;

        } else if(TileSet::isTileFriend(tile)) {
            ADD_TRANS_QUAD(friends);
            auto quad = (trans_quad_batch.instances.end()-1);
            float spr_x = 0;
            switch(TileSet::friendTileToIndex(tile)) {
            case 0: spr_x = 0;    break;
            case 1: spr_x = 483;  break;
            case 2: spr_x = 975;  break;
            case 3: spr_x = 1464; break;
            case 4: spr_x = 1946; break;
            case 5: spr_x = 2430; break;
            case 7: spr_x = 2920; break;
            case 8: spr_x = 3400; break;
            }
            quad->sprite_pos  = vec2(spr_x, 0)/4096.f;
            quad->sprite_size = vec2(480, 4096-3333)/4096.f;
            (friends.end()-1)->m_scale.x = 480.f/(4096-3333);
            (friends.end()-1)->m_scale.x *= .75f;
            (friends.end()-1)->m_scale.y *= .75f;

        } else if(TileSet::isTileKey(tile)) {
            ADD_TRANS_QUAD(keys);
            auto quad = (trans_quad_batch.instances.end()-1);
            quad->sprite_pos  = vec2(512*TileSet::keyTileToIndex(tile),4096-1024)/4096.f;
            quad->sprite_size = vec2(512,512)/4096.f;
        } else if(TileSet::isTileCage(tile)) {
            ADD_CAGE_QUADS(trans_quad_batch);
            continue;
        }
        switch(tile) {
        // Default to shut up GCC's -Wswitch warning.
        default: break;
        case Tile::WALL: ADD_WALL_QUADS(walls_quad_batch); break;
        case Tile::EXIT: ADD_WALL_QUADS(trans_quad_batch); break;
        case Tile::DOOR: ADD_DOOR_QUADS(doors_quad_batch); break;
        }
#undef ADD_WALL_QUAD
#undef ADD_WALL_QUADS
#undef ADD_DOOR_QUAD
#undef ADD_DOOR_QUADS
    }
    walls_quad_batch.updateInstancesVBO();
    doors_quad_batch.updateInstancesVBO();
    trans_quad_batch.updateInstancesVBO();
}



void Dungeon::prepare(size_t i, size_t p_floor_index) {

    walls_quad_batch.instances.clear();
    ground_quad_batch.instances.clear();
    ceiling_quad_batch.instances.clear();
    doors_quad_batch.instances.clear();
    trans_quad_batch.instances.clear();

    dungeon_index = i;

    // Load dungeon at index i...
    switch(i) {
    case 0: case 1:
        walls_quad_batch.texture_unit   = TextureUnit::GRASS_WALL;
        ground_quad_batch.texture_unit  = TextureUnit::GRASS_GROUND;
        ceiling_quad_batch.texture_unit = TextureUnit::MONOSPACE_FONT_ATLAS;
        should_render_ceiling = false;
        break;
    case 2:
        walls_quad_batch.texture_unit   = TextureUnit::CAVE_WALL;
        ground_quad_batch.texture_unit  = TextureUnit::CAVE_GROUND;
        ceiling_quad_batch.texture_unit = TextureUnit::CAVE_WALL;
        should_render_ceiling = true;
        break;
    case 3: case 4:
        walls_quad_batch.texture_unit   = TextureUnit::MANSION_WALL;
        ground_quad_batch.texture_unit  = TextureUnit::MANSION_GROUND;
        ceiling_quad_batch.texture_unit = TextureUnit::MANSION_WALL;
        should_render_ceiling = (i==4);
        break;
    }
    doors_quad_batch.texture_unit = TextureUnit::DOOR;
    trans_quad_batch.texture_unit = TextureUnit::TRANS_ATLAS;

    mat4 rx = rotate(float(M_PI)/2.f, vec3(1,0,0));
    vec2 max_dim(1000, 1000);

    ground_quad_batch.instances.resize(1);
    ground_quad_batch.instances[0].modelmatrix 
        = translate(mat4(), vec3(-.5f+max_dim.x/2, -.5, -.5f+max_dim.y/2))
        * rx
        * scale(vec3(max_dim.x, max_dim.y, 1));
    ground_quad_batch.instances[0].sprite_size = max_dim;
    ground_quad_batch.updateInstancesVBO();

    ceiling_quad_batch.instances.resize(1);
    ceiling_quad_batch.instances[0].modelmatrix 
        = translate(mat4(), vec3(-.5f+max_dim.x/2, .5, -.5f+max_dim.y/2))
        * rx
        * scale(vec3(max_dim.x, max_dim.y, 1));
    ceiling_quad_batch.instances[0].sprite_size = max_dim;
    ceiling_quad_batch.updateInstancesVBO();

    vec3 sky = vec3(0, 175, 255)/255.f;
    if(i<2)
        glClearColor(sky.r, sky.g, sky.b, 1);
    else
        glClearColor(0, 0, .04, 1);

    floor_index = p_floor_index;
    
    // XXX
    bool loaded;
    if(i)
        loaded = tiles.loadFromFile(
            "res/dungeon" + to_string(dungeon_index+1) 
            + "level" + to_string(floor_index+1) + ".ppm");
    else
        loaded = tiles.loadFromFile("res/missingno_e0.ppm");

    is_dungeon_completed = false;
    if(!loaded)
        is_dungeon_completed = true;

    tiles.recomputeInfo();
    hero.position.reset(vec3(tiles.spawn_pos.x, 0, tiles.spawn_pos.y));
    hero.angle_y.reset(tiles.spawn_angle_y);
    hero.speed = 3.2f;
    hero.angular_speed = 2.4f;
    hero.life_max = 100;
    hero.life = hero.life_max;
    hero.attack.launch_speed = 1/.1f;
    hero.attack.post_speed = 1/.5f;
    hero.keys.clear();
    
    fillFloorDataFromTileSet();

    vec3 fogcol = (i<2 ? sky : vec3(0,0,0));
    float fogdist = (i<2 ? 64 : (i==4 ? 6 : 16));
    walls_quad_batch.fog_factor = .8;
    walls_quad_batch.fogdistance = fogdist;
    walls_quad_batch.fogcolor = fogcol;
    ground_quad_batch.fog_factor = .8;
    ground_quad_batch.fogdistance = fogdist;
    ground_quad_batch.fogcolor = fogcol;
    ceiling_quad_batch.fog_factor = .8;
    ceiling_quad_batch.fogdistance = fogdist;
    ceiling_quad_batch.fogcolor = fogcol;
    doors_quad_batch.fog_factor = .8;
    doors_quad_batch.fogdistance = fogdist;
    doors_quad_batch.fogcolor = fogcol;
    trans_quad_batch.fog_factor = .8;
    trans_quad_batch.fogdistance = fogdist;
    trans_quad_batch.fogcolor = fogcol;


    view.position = hero.position;
    view.angle_y  = hero.angle_y;
    sortTransQuadsAndObjects();
    trans_quad_batch.updateInstancesVBO();


    dialogue.lines.clear();
    dialogue.lines.push_back("Z,Q,S,D: move - A,E: turn.");
    dialogue.lines.push_back("[Space] to interact/attack.");
    dialogue.line_height = .064f;
    dialogue.rgba = vec4(1,1,1,1);
    dialogue_box_quad_batch.instances.resize(1);
    dialogue_box_quad_batch.texture_unit = TextureUnit::BLACK_FADER;
    dialogue_box_quad_batch.rgba_fx = vec4(0,0,0,0);
    dialogue_box_quad_batch.alpha_fx_factor = .2;

    string name;
    switch(i) {
    case 0: name = "Small Bush";           break;
    case 1: name = "Sunny Glade";          break;
    case 2: name = "The Great Mine";       break;
    case 3: name = "Sorcerer's Wasteland"; break;
    case 4: name = "Sorcerer's Mansion";   break;
    }
    fader_txt.lines[0] = name;
    fader_txt.lines[1] = "Floor " + to_string(floor_index);
    fader_txt.rgba = vec4(1,1,1,1);

    transition_state = DungeonTransitionState::TEXT_SHOWN_PAUSING;
    fader_opacity = Lerp<float>(1,0);
    fader_text_opacity = Lerp<float>(1,0);
    fader_post_progress = 0;

    reshape(hud_view.viewport_size);
}


void Dungeon::launch(size_t i) {
    prepare(i, 0);
    transition_state = DungeonTransitionState::FADING_IN_TEXT;
    fader_opacity = Lerp<float>(0,1);
    fader_text_opacity = Lerp<float>(0,1);
    fader_post_progress = 0;
}



void Dungeon::sortTransQuadsAndObjects() {
    auto qidx = trans_quad_batch.sortInstancesByDepthKeepingIndices(view.getViewMatrix());

#define FACE_HERO(objvec) \
    for(auto& obj : (objvec)) { \
        auto it = std::find(qidx.begin(), qidx.end(), obj.quad_index); \
        assert(it != qidx.end()); \
        obj.quad_index = it - qidx.begin();\
        obj.face(hero); \
        trans_quad_batch.instances[obj.quad_index].modelmatrix = obj.getModelMatrix(); \
    }
    FACE_HERO(enemies);
    FACE_HERO(friends);
    FACE_HERO(keys);
#undef FACE_HERO
    for(auto& cage : cages) {
        size_t i = 0;
        for(size_t &qindex : cage.quad_indices) {
            auto it = std::find(qidx.begin(), qidx.end(), qindex);
            assert(it != qidx.end());
            qindex = it - qidx.begin();
            trans_quad_batch.instances[qindex].modelmatrix = cage.getQuadModelMatrix(i);
            ++i;
        }
    }
}



// 'taken' receives the planned positions of other AIs and the player, so we know we can't move
// to these.
ivec2 TileObject::decideMoveTowards(const TileObject &target, const TileSet &tiles, const vector<ivec2> &taken) const {
    int my_x = roundf(position.prev.x);
    int my_y = roundf(position.prev.z);
    int tg_x = roundf(target.position.prev.x);
    int tg_y = roundf(target.position.prev.z);
    Tile tile;
    ivec2 dir(0,0);

    // So much duplicated statements, but anyway.
    // Do we see the target in our row ?
    if(my_y == tg_y) {
        if(my_x < tg_x-1) {
            dir.x = 1;
            for(int x=my_x+1 ; x<my_x ; ++x) {
                tile = tiles.getTileAt(x, my_y);
                if(!TileSet::isTileWalkable(tile)) {
                    dir.x = 0;
                    break;
                }
            }
        } else if(my_x > tg_x+1) {
            dir.x = -1;
            for(int x=my_x-1 ; x>my_x ; --x) {
                tile = tiles.getTileAt(x, my_y);
                if(!TileSet::isTileWalkable(tile)) {
                    dir.x = 0;
                    break;
                }
            }
        }
    } else if(my_x == tg_x) {
        if(my_y < tg_y-1) {
            dir.y = 1;
            for(int y=my_y+1 ; y<my_y ; ++y) {
                tile = tiles.getTileAt(my_x, y);
                if(!TileSet::isTileWalkable(tile)) {
                    dir.y = 0;
                    break;
                }
            }
        } else if(my_y > tg_y+1) {
            dir.y = -1;
            for(int y=my_y-1 ; y>my_y ; --y) {
                tile = tiles.getTileAt(my_x, y);
                if(!TileSet::isTileWalkable(tile)) {
                    dir.y = 0;
                    break;
                }
            }
        }
    }

    if(dir != ivec2(0,0)) {
        ivec2 tilepos(my_x + dir.x, my_y + dir.y);
        tile = tiles.getTileAt(tilepos.x, tilepos.y);
        bool is_taken = std::find(taken.begin(), taken.end(), tilepos) != taken.end();
        return !is_taken && TileSet::isTileWalkable(tile) ? dir : ivec2(0,0);
    }

    // Otherwise, do we "feel" the target nearby ?
    vec2 diff = vec2(tg_x, tg_y) - vec2(my_x, my_y);
    if(length(diff) > feel_radius || length(diff) <= 1.1f)
        return ivec2(0,0);

    ivec2 mypos(my_x, my_y);
    // Pull two candidates ivec2.
    ivec2 hdir(diff.x>0 ? 1 : -1, 0);
    ivec2 vdir(0, diff.y>0 ? 1 : -1);
    bool is_h_taken = std::find(taken.begin(), taken.end(), mypos+hdir) != taken.end();
    bool is_v_taken = std::find(taken.begin(), taken.end(), mypos+vdir) != taken.end();
    bool can_move_horizontally = false;
    bool can_move_vertically   = false;
    if(!is_h_taken && TileSet::isTileWalkable(tiles.getTileAt(my_x + hdir.x, my_y)))
        can_move_horizontally = true;
    if(!is_v_taken && TileSet::isTileWalkable(tiles.getTileAt(my_x, my_y + vdir.y)))
        can_move_vertically = true;

    if(abs(diff.x) > abs(diff.y)) {
        if(can_move_horizontally)
            return hdir;
        if(can_move_vertically)
            return vdir;
    } else {
        if(can_move_vertically)
            return vdir;
        if(can_move_horizontally)
            return hdir;
    }
    return ivec2(0,0);
}


// XXX Hack
static bool isFloorIndexAvailable(size_t dungeon_index, size_t floor_index) {
    return floor_index <= dungeon_index;
}



static const float FADER_SPEED = 1.f;
static const float FADER_TEXT_SPEED = 1.f;
static const float FADER_POST_SPEED = 1/2.f;




GameplayType Dungeon::nextFrame(const Input &input, uint32_t fps) {

    if(input.escape)
        return GameplayType::WORLD_MAP;


    switch(transition_state) {
    case DungeonTransitionState::FADING_IN: 
        fader_opacity.progress += FADER_SPEED/fps;
        hud_fader_quad_batch.alpha_fx_factor = 1-fader_opacity.getCurrent();
        if(fader_opacity.progress >= 1) {
            fader_opacity.reset(1);
            transition_state = DungeonTransitionState::FADING_IN_TEXT;
            if(!isFloorIndexAvailable(dungeon_index, floor_index))
                return GameplayType::WORLD_MAP;
        }
        break;
    case DungeonTransitionState::FADING_IN_TEXT:
        fader_text_opacity.progress += FADER_TEXT_SPEED/fps;
        hud_fader_quad_batch.alpha_fx_factor = 0;
        fader_txt.rgba.a = fader_text_opacity.getCurrent();
        fader_txt.updateQuadBatch();
        if(fader_text_opacity.progress >= 1) {
            // XXX Doing it AGAIN when it's the first floor??
            prepare(dungeon_index, floor_index);
            // !!! Do this _after_ prepare()
            fader_text_opacity.reset(1);
            transition_state = DungeonTransitionState::TEXT_SHOWN_PAUSING;
        }
        break;
    case DungeonTransitionState::TEXT_SHOWN_PAUSING: 
        fader_post_progress += FADER_POST_SPEED/fps;
        if(fader_post_progress >= 1) {
            fader_post_progress = 0;
            transition_state = DungeonTransitionState::FADING_OUT;
        }
        break;
    case DungeonTransitionState::FADING_OUT: 
        fader_opacity.progress += FADER_SPEED/fps;
        hud_fader_quad_batch.alpha_fx_factor = 1-fader_opacity.getCurrent();
        fader_txt.rgba.a = fader_opacity.getCurrent();
        fader_txt.updateQuadBatch();
        if(fader_opacity.progress >= 1) {
            fader_opacity.reset(0);
            transition_state = DungeonTransitionState::NONE;
        }
        break;
    // GCC's -Wswitch
    case DungeonTransitionState::NONE: break;
    }

    // Prevent mechanisms from working when we're transitioning
    if(transition_state != DungeonTransitionState::NONE)
        return GameplayType::DUNGEON;



    bool one_enemy_has_moved = false;
    for(size_t i=0 ; i<enemies.size() ; ++i) {
        Enemy &enemy = enemies[i];

        if(!enemy.position.progress) {
            vector<ivec2> taken;
            taken.push_back(hero.getNextTilePos());
            for(size_t j=0 ; j<enemies.size() ; ++j) {
                const Enemy &e = enemies[j];
                if(i != j)
                    taken.push_back(e.getNextTilePos());
            }
            for(size_t j=0 ; j<keys.size() ; ++j) {
                const Key &key = keys[j];
                taken.push_back(key.getTilePos());
            }
            vec2 axis = enemy.decideMoveTowards(hero, tiles, taken);
            enemy.position.next = enemy.position.prev + vec3(axis.x,0,axis.y);
        }
        if(enemy.position.prev != enemy.position.next) {
            enemy.position.progress += enemy.speed/float(fps);
        }

        if(enemy.position.progress > .5f && !enemy.was_tile_swapped) {
            enemy.was_tile_swapped = true;
            size_t x1 = enemy.position.prev.x;
            size_t y1 = enemy.position.prev.z;
            size_t x2 = enemy.position.next.x;
            size_t y2 = enemy.position.next.z;
            tiles.swapTilesAt(x1, y1, x2, y2);
        }

        if(enemy.position.progress >= 1) {
            enemy.position.reset(enemy.position.next);
            enemy.was_tile_swapped = false;
            // Correct the position every time, because errors accumulate
            enemy.position.prev.x = roundf(enemy.position.prev.x);
            enemy.position.prev.y = roundf(enemy.position.prev.y);
            enemy.position.prev.z = roundf(enemy.position.prev.z);
        }


        if(!enemy.attack.launch_progress && !enemy.attack.post_progress) {
            // Attempt an attack only if the hero is within range, and we're
            // not already attacking.
            bool would_hit = enemy.attack.check_hit(enemy.getTilePos(), enemy.angle_y, hero.getTilePos(), tiles);
            /*
            cout << enemy.getTilePos() << " by " 
                << degrees(enemy.angle_y.getCurrent()) << "° "
                << (does_hit ? "Does hit" : "Doesn't hit")
                << " " << hero.getTilePos() << endl;
                */
            if(would_hit) {
                enemy.attack.launch_progress = 0.0001f/float(fps); // Just to start it.
                cout << "Attempting attack..." << endl;
                trans_quad_batch.instances[enemy.quad_index].sprite_pos = enemy.attack_spr_pos;
                trans_quad_batch.updateInstancesVBO();
            }
        }

        if(enemy.attack.launch_progress) {
            enemy.attack.launch_progress += enemy.attack.launch_speed/float(fps);
            if(enemy.attack.launch_progress >= 1) {
                trans_quad_batch.instances[enemy.quad_index].sprite_pos = enemy.idle_spr_pos;
                trans_quad_batch.updateInstancesVBO();
                enemy.attack.launch_progress = 0;
                if(enemy.attack.check_hit(enemy.getTilePos(), enemy.angle_y, hero.getTilePos(), tiles)) {
                    hero.life -= enemy.attack.damage*enemy.damage_multiplier;
                    if(hero.life <= 0)
                        return GameplayType::WORLD_MAP;
                    cout << "Ouch!" << endl;
                    reshape(view.viewport_size);
                }
                enemy.attack.post_progress = 0.0001f/float(fps);
            }
        }

        if(enemy.attack.post_progress) {
            enemy.attack.post_progress += enemy.attack.post_speed/float(fps);
            if(enemy.attack.post_progress >= 1) {
                enemy.attack.post_progress = 0;
                cout << "Attack completed." << endl;
            }
        }

        one_enemy_has_moved = true;
    }


    if(!hero.angle_y.progress)
        hero.angle_y.next = hero.angle_y.prev + input.turnaround*M_PI/2.f;
    if(input.turnaround || hero.angle_y.progress)
        hero.angle_y.progress += hero.angular_speed/float(fps);
    if(hero.angle_y.progress >= 1) {
        hero.angle_y.reset(hero.angle_y.next);
        // Correct the angle every time, because errors accumulate
        hero.angle_y.prev = angleClosestQuarter(hero.angle_y.prev);
    }

    // Have our own axis, which disallows moving diagonally.
    ivec2 axis = input.axis;
    if(input.axis.x && input.axis.y) {
        axis.x = 0;
    }

   
    if(input.interact) {

        vec3 direction = rotate(mat3(), hero.angle_y.getCurrent())
                       * vec3(1, 0, 0);
        direction.y = -direction.y;
        std::swap(direction.y, direction.z);
        vec3 forward_pos = hero.position.getCurrent() + direction;
        Tile forward_tile = tiles.getTileAt(forward_pos.x, forward_pos.z);
        size_t x2 = forward_pos.x;
        size_t y2 = forward_pos.z;

        if(TileSet::isTileCage(forward_tile)) {
            auto key = std::find(hero.keys.begin(), hero.keys.end(), TileSet::cageToKey(forward_tile));
            if(key != hero.keys.end()) {
                auto cage = cages.findByTilePos(x2, y2);
                cage->position.reset(vec3(-1000, -1000, -1000));
                sortTransQuadsAndObjects();
                trans_quad_batch.updateInstancesVBO();
                cages.erase(cage);
                hero.keys.erase(key);
                reshape(hud_view.viewport_size);
                tiles.setTileAt(x2, y2, Tile::GROUND);
                dialogue.lines[0] = "Yay! Well done!";
                dialogue.lines[1] = "";
                dialogue.updateQuadBatch();
            } else {
                dialogue.lines[0] = "Nope! You need a key.";
                dialogue.lines[1] = "";
                dialogue.updateQuadBatch();
            }

        } else if(forward_tile == Tile::DOOR) {
            auto door = doors.findByTilePos(x2, y2);
            door->opening_angle.progress = 0.0001f/float(fps); // Just to start it.
            // cout << "Door's angle: " << degrees(door->angle_y.getCurrent()) << "°" << endl;
            // cout << "Hero's angle: " << degrees(hero.angle_y.getCurrent()) << "°" << endl;
            float doorquarter = angleClosestQuarter(door->angle_y);
            float heroquarter = angleClosestQuarter(hero.angle_y);

            door->opening_angle.next = (doorquarter==heroquarter ? M_PI/2 : -M_PI/2);
            tiles.setTileAt(x2, y2, Tile::GROUND);
            dialogue.lines[0] = "Opened door.";
            dialogue.lines[1] = "Wonder what's behind...";
            dialogue.updateQuadBatch();

        } else { // Neither cage nor door
            if(!hero.attack.launch_progress && !hero.attack.post_progress) {
                hero.attack.launch_progress = 0.0001f/float(fps); // Just to start it.
                cout << "Hero: Attempting attack..." << endl;
            }
        }
    }

    if(hero.attack.launch_progress) {
        hero.attack.launch_progress += hero.attack.launch_speed/float(fps);
        if(hero.attack.launch_progress >= 1) {
            hero.attack.launch_progress = 0;
            hero.attack.post_progress = 0.0001f/float(fps);
            for(Enemy &enemy : enemies) {
                bool would_hit = hero.attack.check_hit(hero.getTilePos(), hero.angle_y, enemy.getTilePos(), tiles);
                if(would_hit) {
                    cout << "Gotcha!" << endl;
                    enemy.life -= hero.attack.damage*hero.damage_multiplier;
                    if(enemy.life <= 0) {
                        ivec2 epos = enemy.getTilePos();
                        tiles.setTileAt(epos.x, epos.y, Tile::GROUND);
                        enemy.position = vec3(-1000, -1000, -1000);
                        sortTransQuadsAndObjects();
                        trans_quad_batch.updateInstancesVBO();
                    }
                }
            }
        }
    }

    if(hero.attack.post_progress) {
        hero.attack.post_progress += hero.attack.post_speed/float(fps);
        if(hero.attack.post_progress >= 1) {
            hero.attack.post_progress = 0;
            cout << "Hero: Attack completed." << endl;
        }
    }



    if(!hero.position.progress) {
        float mov_angle = hero.angle_y.getCurrent();
        if(hero.angle_y.progress)
            mov_angle = angleClosestQuarter(mov_angle);

        vec3 mov = rotate(mat3(), mov_angle - float(M_PI)/2.f)
                 * vec3(axis.x, axis.y, 0);

        vec3 next_pos = hero.position.prev + vec3(mov.x, 0, -mov.y);
        bool is_tile_taken = false;
        for(Enemy &enemy : enemies) {
            if(roundf(enemy.position.next.x) == roundf(next_pos.x)
            && roundf(enemy.position.next.z) == roundf(next_pos.z)) {
                is_tile_taken = true;
                break;
            }
        }

        Tile tile = tiles.getTileAt(next_pos.x, next_pos.z);
        if(TileSet::isTileWalkable(tile) && !is_tile_taken)
            hero.position.next = next_pos;
    }
    if(hero.position.prev != hero.position.next) {
        hero.position.progress += hero.speed/float(fps);
    }


    if(hero.position.progress > .5f && !hero.was_tile_swapped) {
        hero.was_tile_swapped = true;
        size_t x1 = hero.position.prev.x;
        size_t y1 = hero.position.prev.z;
        size_t x2 = hero.position.next.x;
        size_t y2 = hero.position.next.z;
        Tile tile = tiles.getTileAt(x2, y2);
        if(TileSet::isTileKey(tile)) {
            auto key = keys.findByTilePos(x2, y2);
            key->position.reset(vec3(-1000, -1000, -1000));
            sortTransQuadsAndObjects();
            trans_quad_batch.updateInstancesVBO();
            keys.erase(key);

            hero.keys.push_back(tile);
            std::sort(hero.keys.begin(), hero.keys.end());
            reshape(hud_view.viewport_size);
            tiles.setTileAt(x2, y2, Tile::GROUND);
            tiles.swapTilesAt(x1, y1, x2, y2);
        } else if(tile == Tile::EXIT) {
            if(cages.size()) {
                dialogue.lines[0] = "You need to open every cage";
                dialogue.lines[1] = "before moving on.";
                dialogue.updateQuadBatch();
            } else {
                transition_state = DungeonTransitionState::FADING_IN;
                fader_opacity = Lerp<float>(0,1);
                fader_text_opacity = Lerp<float>(0,1);
                ++floor_index;
                return GameplayType::DUNGEON;
            }
        }
    }

    if(hero.position.progress >= 1) {
        hero.position.reset(hero.position.next);
        hero.was_tile_swapped = false;
        // Correct the position every time, because errors accumulate
        hero.position.prev.x = roundf(hero.position.prev.x);
        hero.position.prev.y = roundf(hero.position.prev.y);
        hero.position.prev.z = roundf(hero.position.prev.z);
    }


    bool one_door_opening = false;
    for(Door& door : doors) {
        if(door.opening_angle.progress) {
            one_door_opening = true;
            door.opening_angle.progress += Door::OPENING_SPEED;
            if(door.opening_angle.progress >= 1)
                door.opening_angle.reset(door.opening_angle.next);
            doors_quad_batch.instances[door.l_quad_index].modelmatrix = door.getLModelMatrix();
            doors_quad_batch.instances[door.r_quad_index].modelmatrix = door.getRModelMatrix();
        }
    }
    if(one_door_opening)
        doors_quad_batch.updateInstancesVBO();


    if(hero.position.progress || hero.angle_y.progress || one_enemy_has_moved) {
        sortTransQuadsAndObjects();
        trans_quad_batch.updateInstancesVBO();
    }

    view.position = hero.position;
    view.angle_y  = hero.angle_y;


    return GameplayType::DUNGEON;
}

void Dungeon::renderGL() const {
    walls_quad_batch.renderGL(view);
    doors_quad_batch.renderGL(view);
    ground_quad_batch.renderGL(view);
    if(should_render_ceiling)
        ceiling_quad_batch.renderGL(view);
    trans_quad_batch.renderGL(view);

    hud_keys_quad_batch.renderGL_HUD(hud_view);
    hud_life_quad_batch.renderGL_HUD(hud_view);
    dialogue_box_quad_batch.renderGL_HUD(hud_view);
    dialogue.renderGL_HUD(hud_view);
    hud_fader_quad_batch.renderGL_HUD(hud_view);
    fader_txt.renderGL_HUD(hud_view);
}

} // namespace dm
