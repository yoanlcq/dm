#include <dm/dm.hpp>

using namespace std;
using namespace glm;

namespace dm {


Tile TileSet::rgb24ToTile(rgb24 col) {
    uint32_t tile = (col.r<<16) | (col.g<<8) | col.b;
    return (Tile)tile;
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


size_t Dungeon::refcount(0);
GLuint Dungeon::tex_grass_ground  (0);
GLuint Dungeon::tex_grass_wall    (0);
GLuint Dungeon::tex_grass_sky     (0);
GLuint Dungeon::tex_cave_ground   (0);
GLuint Dungeon::tex_cave_wall     (0);
GLuint Dungeon::tex_mansion_ground(0);
GLuint Dungeon::tex_mansion_wall  (0);

void Dungeon::setupGL() {
    hope(tex_grass_ground   = GLTexture_fromFile("res/tex_sol_prairie.jpg"));
    hope(tex_grass_wall     = GLTexture_fromFile("res/tex_mur_prairie.png"));
    hope(tex_grass_sky      = GLTexture_fromFile("res/tex_ciel_prairie.png"));
    hope(tex_cave_ground    = GLTexture_fromFile("res/tex_sol_grotte.jpg"));
    hope(tex_cave_wall      = GLTexture_fromFile("res/tex_mur_grotte.jpg"));
    hope(tex_mansion_ground = GLTexture_fromFile("res/tex_sol_manoir.jpg"));
    hope(tex_mansion_wall   = GLTexture_fromFile("res/tex_mur_manoir.jpg"));

    GLTexture_bindToUnit(tex_grass_ground  , TextureUnit::GRASS_GROUND);
    GLTexture_bindToUnit(tex_grass_wall    , TextureUnit::GRASS_WALL);
    GLTexture_bindToUnit(tex_grass_sky     , TextureUnit::GRASS_SKY);
    GLTexture_bindToUnit(tex_cave_ground   , TextureUnit::CAVE_GROUND);
    GLTexture_bindToUnit(tex_cave_wall     , TextureUnit::CAVE_WALL);
    GLTexture_bindToUnit(tex_mansion_ground, TextureUnit::MANSION_GROUND);
    GLTexture_bindToUnit(tex_mansion_wall  , TextureUnit::MANSION_WALL);
}
void Dungeon::cleanupGL() {
    glDeleteTextures(1, &tex_grass_ground  );
    glDeleteTextures(1, &tex_grass_wall    );
    glDeleteTextures(1, &tex_grass_sky     );
    glDeleteTextures(1, &tex_cave_ground   );
    glDeleteTextures(1, &tex_cave_wall     );
    glDeleteTextures(1, &tex_mansion_ground);
    glDeleteTextures(1, &tex_mansion_wall  );
}

Dungeon:: Dungeon(ivec2 viewport_size) 
    : view(viewport_size),
      hud_view(viewport_size)
{
    if(!refcount)
        setupGL();
    ++refcount;
}
Dungeon::~Dungeon() {
    --refcount;
    if(!refcount)
        cleanupGL();
}


void Dungeon::reshape(ivec2 new_viewport_size) {
    
    view.reshape(new_viewport_size);
    hud_view.reshape(new_viewport_size);

    dialogue.position = vec3(
        -dialogue.getLineWidth(0)/2.f,
        -hud_view.getHalfHeight() + 2.f*dialogue.line_height,
        0
    );
    dialogue.updateQuadBatch();

    dialogue_box_quad_batch.instances[0].modelmatrix 
        = translate(mat4(), vec3(0, 2.f*dialogue.line_height-hud_view.getHalfHeight(), 0))
        * scale(vec3(1.1f, 1.8f, 1))
        * scale(vec3(dialogue.getLineWidth(0), 2.f*dialogue.line_height, 1));
    dialogue_box_quad_batch.updateInstancesVBO();
}


void TileObject::face(const TileObject &other) {
    // Kinda hard to choose betwwen the two.
#if 1 
    vec3 facing_vec = position.getCurrent() - other.position.getCurrent();
    angle_y = atan2f(-facing_vec.z, facing_vec.x);
#else
    angle_y = other.angle_y + M_PI;
#endif
}

mat4 TileObject::getModelMatrix() const {
    return translate(mat4(), position.getCurrent())
         * rotate(angle_y-float(M_PI)/2.f, vec3(0,1,0));
}

void Dungeon::fillFloorDataFromTileSet() {

    enemies.clear();
    friends.clear();
    keys.clear();
    cages.clear();

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
            (qbatch).instances.push_back(quad); \
        }
#define ADD_WALL_QUADS(qbatch) { \
            if(x+1 < tiles.w) if(tiles.getTileAt(x+1, y) != Tile::WALL) ADD_WALL_QUAD((qbatch), 0.f); \
            if(x > 0)         if(tiles.getTileAt(x-1, y) != Tile::WALL) ADD_WALL_QUAD((qbatch), M_PI); \
            if(y+1 < tiles.h) if(tiles.getTileAt(x, y+1) != Tile::WALL) ADD_WALL_QUAD((qbatch), 3*M_PI/2); \
            if(y > 0)         if(tiles.getTileAt(x, y-1) != Tile::WALL) ADD_WALL_QUAD((qbatch), M_PI/2); \
        }
#define ADD_DOOR_QUAD(qbatch, quad_angle_y) { \
            quad.modelmatrix = translate(mat4(), vec3(x,0,y)) \
                             * rotate(float(quad_angle_y)-float(M_PI)/2.f, vec3(0,1,0)) \
                             * translate(mat4(), vec3(-.25f, 0, 0)) \
                             * scale(vec3(.5f, 1, 1)); \
            (qbatch).instances.push_back(quad); \
            quad.modelmatrix = translate(mat4(), vec3(x,0,y)) \
                             * rotate(float(quad_angle_y)-float(M_PI)/2.f, vec3(0,1,0)) \
                             * translate(mat4(), vec3(+.25f, 0, 0)) \
                             * scale(vec3(.5f, 1, 1)); \
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
        } else if(TileSet::isTileFriend(tile)) {
            ADD_TRANS_QUAD(friends);
        } else if(TileSet::isTileKey(tile)) {
            ADD_TRANS_QUAD(keys);
        } else if(TileSet::isTileCage(tile)) {
            ADD_WALL_QUADS(trans_quad_batch);
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


void Dungeon::prepare(size_t i) {

    walls_quad_batch.instances.clear();
    ground_quad_batch.instances.clear();
    ceiling_quad_batch.instances.clear();
    doors_quad_batch.instances.clear();
    trans_quad_batch.instances.clear();

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
    doors_quad_batch.texture_unit = TextureUnit::WORLD_MAP;
    trans_quad_batch.texture_unit = TextureUnit::MONOSPACE_FONT_ATLAS;

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

    hero.speed = 3.2f;
    hero.angular_speed = 2.4f;
    floor_index = 0;
    
    hope(tiles.loadFromFile("res/missingno_e" + to_string(floor_index) + ".ppm"));
    tiles.recomputeInfo();
    hero.position.reset(vec3(tiles.spawn_pos.x, 0, tiles.spawn_pos.y));
    hero.angle_y.reset(tiles.spawn_angle_y);
    
    fillFloorDataFromTileSet();

    vec3 fogcol = (i<2 ? sky : vec3(0,0,0));
    float fogdist = (i<2 ? 64 : 16);
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

    trans_quad_batch.sortInstancesByDepth(view.getViewMatrix());

    dialogue.lines.push_back("Oh no! The princess was kidnapped!");
    dialogue.lines.push_back("Please save her!");
    dialogue.line_height = .064f;
    dialogue.rgba = vec4(1,1,1,1);
    dialogue_box_quad_batch.instances.resize(1);
    dialogue_box_quad_batch.texture_unit = TextureUnit::WORLD_MAP_FADER; // XXX intent not clear
    dialogue_box_quad_batch.rgba_fx = vec4(0,0,0,0);
    dialogue_box_quad_batch.alpha_fx_factor = .2;

    reshape(hud_view.viewport_size);
}

static float angleClosestQuarter(float angle) {
    return 2.f*M_PI*roundf(angle/(M_PI/2.f))/4.f;
}

GameplayType Dungeon::nextFrame(const Input &input, uint32_t fps) {

    if(input.escape)
        return GameplayType::WORLD_MAP;

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

    if(!hero.position.progress) {
        float mov_angle = hero.angle_y.getCurrent();
        if(hero.angle_y.progress)
            mov_angle = angleClosestQuarter(mov_angle);

        vec3 mov = rotate(mat3(), mov_angle - float(M_PI)/2.f)
                 * vec3(axis.x, axis.y, 0);

        vec3 next_pos = hero.position.prev + vec3(mov.x, 0, -mov.y);
        if(TileSet::isTileWalkable(tiles.getTileAt(next_pos.x, next_pos.z)))
            hero.position.next = next_pos;
    }
    if(axis.x || axis.y || hero.position.progress) {
        hero.position.progress += hero.speed/float(fps);
    }
    if(hero.position.progress >= 1) {
        hero.position.reset(hero.position.next);
        // Correct the position every time, because errors accumulate
        hero.position.prev.x = roundf(hero.position.prev.x);
        hero.position.prev.y = roundf(hero.position.prev.y);
        hero.position.prev.z = roundf(hero.position.prev.z);
    }


    if(hero.position.progress || hero.angle_y.progress) {
        auto qidx = trans_quad_batch.sortInstancesByDepthKeepingIndices(view.getViewMatrix());

#define FACE_HERO(objvec) \
        for(auto& obj : (objvec)) { \
            obj.quad_index = std::find(qidx.begin(), qidx.end(), obj.quad_index) - qidx.begin();\
            obj.face(hero); \
            trans_quad_batch.instances[obj.quad_index].modelmatrix = obj.getModelMatrix(); \
        }
        FACE_HERO(enemies);
        FACE_HERO(friends);
        FACE_HERO(keys);
#undef FACE_HERO
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

    dialogue_box_quad_batch.renderGL_HUD(hud_view);
    dialogue.renderGL_HUD(hud_view);
}

} // namespace dm
