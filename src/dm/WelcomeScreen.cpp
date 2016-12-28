#include <dm/dm.hpp>

using namespace glm;
using namespace std;

namespace dm {


size_t WelcomeScreen::refcount(0);
GLuint WelcomeScreen::tex(0);


void WelcomeScreen::setupGL() {
    hope(tex = GLTexture_fromFile("res/welcome_screen.png"));
    GLTexture_bindToUnit(tex, TextureUnit::WELCOME_SCREEN);
}
void WelcomeScreen::cleanupGL() {
    glDeleteTextures(1, &tex);
}


WelcomeScreen:: WelcomeScreen(ivec2 viewport_size) 
    : view(viewport_size),
      bg_color(vec3(1,1,1), vec3(0,0,0)),
      is_in_transition(false)
{
    if(!refcount)
        setupGL();
    ++refcount;

    GLQuadBatch::QuadInstance quad = {
        scale(vec3(1,1,1)), vec2(0,0), vec2(1,1)
    };
    quad_batch.instances.push_back(quad);
    quad_batch.texture_unit = TextureUnit::WELCOME_SCREEN;
    quad_batch.updateInstancesVBO();
    quad_batch.rgba_fx = vec4(0,0,0,0);
}

WelcomeScreen::~WelcomeScreen() {
    --refcount;
    if(!refcount)
        cleanupGL();
}

void WelcomeScreen::reshape(ivec2 new_viewport_size) {
    view.reshape(new_viewport_size);
}

GameplayType WelcomeScreen::nextFrame(const Input &input, uint32_t fps) {
    if(input.interact)
        is_in_transition = true;

    if(is_in_transition) {
        bg_color.progress += 1.4f/fps;
        quad_batch.rgba_fx_factor = clamp<float>(bg_color.progress,0,1);
    }

    vec3 c = bg_color.getCurrent();
    glClearColor(c.x, c.y, c.z, 1);

    return bg_color.progress>=1.6f ? GameplayType::WORLD_MAP : GameplayType::WELCOME_SCREEN;
}

void WelcomeScreen::renderGL() const {
    quad_batch.renderGL_HUD(view);
}

} // namespace dm
