#include <dm/dm.hpp>

using namespace glm;
using namespace std;

namespace dm {

size_t WorldMap::refcount(0);
GLuint WorldMap::tex(0);
GLuint WorldMap::fader_tex(0);

void WorldMap::setupGL() {
    hope(tex       = GLTexture_fromFile("res/world_map.png"));
    hope(fader_tex = GLTexture_fromRgba32(rgba32(0,0,0,255)));
    GLTexture_bindToUnit(tex,       TextureUnit::WORLD_MAP);
    GLTexture_bindToUnit(fader_tex, TextureUnit::WORLD_MAP_FADER);
}

void WorldMap::cleanupGL() {
    glDeleteTextures(1, &tex);
    glDeleteTextures(1, &fader_tex);
}


    // Hardcoded FTW.
static const float bg_h(562/float(1024));
static const float bg_y(1-bg_h);
static const float bg_ratio(1.f/bg_h);
static const vec2  hero_sz(294/1024.f, 1-(1364/2048.f));
static const float hero_ratio(hero_sz.x/hero_sz.y);


// These coords were taken with a window size of (800, 600).
static const vec3 NODE_POS[WorldMap::NODE_COUNT] = {
    vec3(-1.015f, -0.254f, 0),
    vec3(-0.545f, -0.254f, 0),
    vec3(-0.045f, -0.254f, 0),
    vec3( 0.505f, -0.254f, 0),
    vec3( 1.115f, -0.254f, 0)
};

static vec3 getNodePos(WorldMap *wm, size_t i) {
    float old_ratio(800/600.f);
    float new_ratio(wm->view.getAspectRatio());
    return NODE_POS[i] * old_ratio / new_ratio;
}

static const float node_text_initial_alphas[WorldMap::NODE_COUNT] = {
    1, 1, .8, .95, 1
};

WorldMap:: WorldMap(ivec2 viewport_size)
    : view(viewport_size),
      fader_opacity(1,0),
      current_node_index(0),
      next_node_index(0),
      hero_position(getNodePos(this, current_node_index)),
      is_starting_dungeon(false)
{
    if(!refcount)
        setupGL();
    ++refcount;

    GLQuadBatch::QuadInstance bg_quad = {
        mat4(), vec2(0,bg_y), vec2(1,bg_h)
    };
    bg_quad_batch.instances.push_back(bg_quad);
    bg_quad_batch.texture_unit = TextureUnit::WORLD_MAP;
    bg_quad_batch.updateInstancesVBO();

    GLQuadBatch::QuadInstance hero_quad = {
        mat4(), vec2(0,0), hero_sz
    };
    hero_quad_batch.instances.push_back(hero_quad);
    hero_quad_batch.texture_unit = TextureUnit::WORLD_MAP;
    hero_quad_batch.updateInstancesVBO();

    GLQuadBatch::QuadInstance fader_quad = {
        scale(vec3(9000,9000,1)), vec2(0,0), vec2(1,1)
    };
    fader_quad_batch.instances.push_back(fader_quad);
    fader_quad_batch.texture_unit = TextureUnit::WORLD_MAP_FADER;
    fader_quad_batch.updateInstancesVBO();
    fader_quad_batch.rgba_fx = vec4(0,0,0,0);

    node_text[0].lines.push_back("Forest of Happy");
    node_text[0].rgba = vec4(0,.3,.1,0);
    node_text[1].lines.push_back("Glade of Happy");
    node_text[1].rgba = vec4(0,.3,.1,0);
    node_text[2].lines.push_back("The Great Mine");
    node_text[2].rgba = vec4(0,0,0,0);
    node_text[3].lines.push_back("Sorcerer's Wasteland");
    node_text[3].rgba = vec4(1,1,1,0);
    node_text[4].lines.push_back("Sorcerer's Mansion");
    node_text[4].rgba = vec4(1,.1,0,0);

    view.position.prev.x -= 9000; // Go as left as possible, nextFrame() will fix it.

    reshape(viewport_size);
}

WorldMap::~WorldMap() {
    --refcount;
    if(!refcount)
        cleanupGL();
}

void WorldMap::reshape(ivec2 new_viewport_size) {
    view.reshape(new_viewport_size);
    const float vph = view.getHalfHeight();

    mat4 &mdl = bg_quad_batch.instances[0].modelmatrix;
    const float bg_scale = 2.f;
    mdl = scale(vec3(bg_scale*vph*bg_ratio, bg_scale*vph, 1));
    bg_quad_batch.updateInstancesVBO();

    hero_position.prev = getNodePos(this, current_node_index);
    hero_position.next = getNodePos(this, next_node_index);
    mat4 &hero_mdl = hero_quad_batch.instances[0].modelmatrix;
    const float hero_scale = bg_scale/6.f;
    hero_mdl = translate(mat4(), hero_position.getCurrent())
             * scale(vec3(hero_scale*vph*hero_ratio, hero_scale*vph, 1));
    hero_quad_batch.updateInstancesVBO();

    for(uint_fast32_t i=0 ; i<WorldMap::NODE_COUNT ; ++i) {
        node_text[i].height = .05f * (800/600.f) / view.getAspectRatio();
        node_text[i].position = getNodePos(this, i);
        node_text[i].position.x -= node_text[i].getLineWidth(0)/2.f;
        node_text[i].position.y -= node_text[i].height*4.2f;
        node_text[i].updateQuadBatch();
    }
}

GameplayType WorldMap::nextFrame(const Input &input, uint32_t fps) {

    glClearColor(0,1,0,1);
    
    if(input.interact && hero_position.progress <= 0 && fader_opacity.progress >= 1) {
        is_starting_dungeon = true;
        fader_opacity.progress = 1;
    }

    if(is_starting_dungeon) {
        fader_opacity.progress -= 1.6f/fps;
        if(fader_opacity.progress <= 0) {
            is_starting_dungeon = false;
            return GameplayType::DUNGEON;
        }
    } else
        fader_opacity.progress += 1.6f/fps;

    fader_quad_batch.rgb_fx_factor   = 1.f-fader_opacity.getCurrent();
    fader_quad_batch.alpha_fx_factor = 1.f-fader_opacity.getCurrent();


    if(next_node_index == current_node_index && !is_starting_dungeon) {
        next_node_index = clamp<int>(current_node_index+input.axis.x, 0, NODE_COUNT-1);
        hero_position.next = getNodePos(this, next_node_index);
    }
    // Not "else" here.
    if(next_node_index != current_node_index)
        hero_position.progress += 1.6f/fps;

    if(hero_position.progress >= 1) {
        current_node_index = next_node_index;
        hero_position.reset(getNodePos(this, current_node_index));
    }
    node_text[next_node_index]   .rgba.a = node_text_initial_alphas[next_node_index]   *(  hero_position.progress);
    node_text[current_node_index].rgba.a = node_text_initial_alphas[current_node_index]*(1-hero_position.progress);

    const vec4 castle_darken_color(0, 0, .05f, 1);
    const float node3_fx_factor(0.4f);
    const float node4_fx_factor(0.8f);

    // Darken the hero if we reach the last nodes
    if(current_node_index==2 && next_node_index==3) {
        hero_quad_batch.rgba_fx = castle_darken_color;
        hero_quad_batch.rgb_fx_factor = node3_fx_factor*hero_position.progress;
    }
    if(current_node_index==3 && next_node_index==2) {
        hero_quad_batch.rgba_fx = castle_darken_color;
        hero_quad_batch.rgb_fx_factor = node3_fx_factor*(1-hero_position.progress);
    }
    if(current_node_index==3 && next_node_index==4) {
        hero_quad_batch.rgba_fx = castle_darken_color;
        hero_quad_batch.rgb_fx_factor = lerpf(node3_fx_factor, node4_fx_factor, hero_position.progress);
    }
    if(current_node_index==4 && next_node_index==3) {
        hero_quad_batch.rgba_fx = castle_darken_color;
        hero_quad_batch.rgb_fx_factor = lerpf(node4_fx_factor, node3_fx_factor, hero_position.progress);
    }


    // XXX Hack to move the hero
    reshape(view.viewport_size);

    view.position.prev.x = hero_position.getCurrent().x;
    const float rat = bg_ratio/view.getAspectRatio();
    clamp_ref<float>(view.position.prev.x, 1-rat, rat-1);

    return GameplayType::WORLD_MAP;
}

void WorldMap::renderGL() const {
    bg_quad_batch.renderGL_HUD(view);
    hero_quad_batch.renderGL_HUD(view);
    for(uint_fast32_t i=0 ; i<WorldMap::NODE_COUNT ; ++i)
        node_text[i].renderGL_HUD(view);
    fader_quad_batch.renderGL_HUD(view);
}

} // namespace dm
