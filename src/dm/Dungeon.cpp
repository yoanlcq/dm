#include <dm/dm.hpp>

using namespace std;
using namespace glm;

namespace dm {

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

Dungeon:: Dungeon(ivec2 viewport_size) : view(viewport_size) {
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
}

void Dungeon::prepare(size_t i) {
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
        ceiling_quad_batch.texture_unit = TextureUnit::CAVE_GROUND;
        should_render_ceiling = true;
        break;
    case 3: case 4:
        walls_quad_batch.texture_unit   = TextureUnit::MANSION_WALL;
        ground_quad_batch.texture_unit  = TextureUnit::MANSION_GROUND;
        ceiling_quad_batch.texture_unit = TextureUnit::MANSION_GROUND;
        should_render_ceiling = (i==4);
        break;
    }
    walls_quad_batch.instances.resize(4);
    walls_quad_batch.instances[0].modelmatrix = translate(mat4(), vec3( 0,0,-1.5));
    walls_quad_batch.instances[1].modelmatrix = translate(mat4(), vec3(-1,0,-1.5));
    walls_quad_batch.instances[2].modelmatrix = translate(mat4(), vec3( 0,1,-1.5));
    walls_quad_batch.instances[3].modelmatrix = translate(mat4(), vec3( 1,0,-1.5));
    walls_quad_batch.updateInstancesVBO();

    mat4 rx = rotate(float(M_PI)/2.f, vec3(1,0,0));
    vec2 max_dim(1000, 1000);

    ground_quad_batch.instances.resize(1);
    ground_quad_batch.instances[0].modelmatrix 
        = translate(mat4(), vec3( 0,-.5, 0))
        * rx
        * scale(vec3(max_dim.x, max_dim.y, 1));
    ground_quad_batch.instances[0].sprite_size = max_dim;
    ground_quad_batch.updateInstancesVBO();

    ceiling_quad_batch.instances.resize(1);
    ceiling_quad_batch.instances[0].modelmatrix 
        = translate(mat4(), vec3( 0,0.5, 0))
        * rx
        * scale(vec3(max_dim.x, max_dim.y, 1));
    ceiling_quad_batch.instances[0].sprite_size = max_dim;
    ceiling_quad_batch.updateInstancesVBO();

    if(i<2) {
        vec3 sky = vec3(158, 209, 245)/255.f;
        glClearColor(sky.r, sky.g, sky.b, 1);
    } else
        glClearColor(0, 0, .04, 1);
}

GameplayType Dungeon::nextFrame(const Input &input, uint32_t fps) {
    const float move_speed(3.2f); // tiles per second
    const float turn_speed(1.4f); // quarters per second
    view.angle_y.prev += input.turnaround*turn_speed*M_PI*.5f/float(fps);
    vec3 mov = rotate(mat3(), view.angle_y.prev - float(M_PI)/2.f)
             * (vec3(input.axis.x, input.axis.y, 0)*move_speed/float(fps));
    view.position.prev.x += mov.x;
    view.position.prev.z -= mov.y;

    if(input.escape)
        return GameplayType::WORLD_MAP;
    return GameplayType::DUNGEON;
}
void Dungeon::renderGL() const {
    walls_quad_batch.renderGL(view);
    ground_quad_batch.renderGL(view);
    if(should_render_ceiling)
        ceiling_quad_batch.renderGL(view);
}

} // namespace dm
