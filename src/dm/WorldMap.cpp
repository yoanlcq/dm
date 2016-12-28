#include <dm/dm.hpp>

using namespace glm;
using namespace std;

namespace dm {

size_t WorldMap::refcount(0);
GLuint WorldMap::bg_tex(0);
GLuint WorldMap::hero_tex(0);
GLuint WorldMap::fader_tex(0);

void WorldMap::setupGL() {
    hope(bg_tex    = GLTexture_fromFile("res/world_map.png"));
    hope(hero_tex  = GLTexture_fromFile("res/world_map_hero.png"));
    hope(fader_tex = GLTexture_fromRgba32(rgba32(0,0,0,255)));
    GLTexture_bindToUnit(bg_tex,    TextureUnit::WORLD_MAP_BG);
    GLTexture_bindToUnit(hero_tex,  TextureUnit::WORLD_MAP_HERO);
    GLTexture_bindToUnit(fader_tex, TextureUnit::WORLD_MAP_FADER);
}

void WorldMap::cleanupGL() {
    glDeleteTextures(1, &bg_tex);
    glDeleteTextures(1, &hero_tex);
    glDeleteTextures(1, &fader_tex);
}



WorldMap:: WorldMap(ivec2 viewport_size)
    : view(viewport_size),
      fader_opacity(1,0)
{
    if(!refcount)
        setupGL();
    ++refcount;

#if 1
    // Hardcoded FTW.
    const float spr_h(562/float(1024));
    const float spr_y(1-spr_h);
    GLQuadBatch::QuadInstance bg_quad = {
        scale(vec3(2,2*spr_h,1)), vec2(0,spr_y), vec2(1,spr_h)
    };
#else
    // Testing rendering some sprite.
    glm::vec2 bl = vec2(45, 1024-706)/1024.f;
    glm::vec2 sz = vec2(102-45, 706-580)/1024.f;
    GLQuadBatch::QuadInstance bg_quad = {
        translate(scale(vec3(sz.x,sz.y,1)), vec3(-2,0,0)), bl, sz
    };
#endif
    bg_quad_batch.instances.push_back(bg_quad);
    bg_quad_batch.texture_unit = TextureUnit::WORLD_MAP_BG;
    bg_quad_batch.updateInstancesVBO();

    GLQuadBatch::QuadInstance hero_quad = {
        translate(scale(vec3(.2,.2,1)), vec3(-3.85,-.9,0)), vec2(0,0), vec2(1,1)
    };
    hero_quad_batch.instances.push_back(hero_quad);
    hero_quad_batch.texture_unit = TextureUnit::WORLD_MAP_HERO;
    hero_quad_batch.updateInstancesVBO();

    GLQuadBatch::QuadInstance fader_quad = {
        scale(vec3(9000,9000,1)), vec2(0,0), vec2(1,1)
    };
    fader_quad_batch.instances.push_back(fader_quad);
    fader_quad_batch.texture_unit = TextureUnit::WORLD_MAP_FADER;
    fader_quad_batch.updateInstancesVBO();
    fader_quad_batch.rgba_fx = vec4(0,0,0,0);
}

WorldMap::~WorldMap() {
    --refcount;
    if(!refcount)
        cleanupGL();
}


void WorldMap::reshape(ivec2 new_viewport_size) {
    view.reshape(new_viewport_size);
}

GameplayType WorldMap::nextFrame(const Input &input, uint32_t fps) {
    glClearColor(0,1,0,1);
    fader_opacity.progress += 1.6f/fps;
    fader_quad_batch.rgba_fx_factor = 1.f-fader_opacity.getCurrent();
    return GameplayType::WORLD_MAP;
}

void WorldMap::renderGL() const {
    bg_quad_batch.renderGL_HUD(view);
    hero_quad_batch.renderGL_HUD(view);
    fader_quad_batch.renderGL_HUD(view);
}

} // namespace dm
