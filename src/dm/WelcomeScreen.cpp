#include <dm/dm.hpp>

using namespace glm;
using namespace std;

namespace dm {


size_t WelcomeScreen::refcount(0);
GLuint WelcomeScreen::tex(0);


void WelcomeScreen::setupGL() {
    hope(tex = GLTexture_fromFile("res/welcome.png"));
    GLTexture_bindToUnit(tex, TextureUnit::WELCOME_SCREEN);
}
void WelcomeScreen::cleanupGL() {
    glDeleteTextures(1, &tex);
}

static const float welcome_h = 1152/2048.f;

WelcomeScreen:: WelcomeScreen(ivec2 viewport_size) 
    : view(viewport_size),
      bg_color(vec3(1,1,1), vec3(0,0,0)),
      is_in_transition(false)
{
    if(!refcount)
        setupGL();
    ++refcount;

    GLQuadBatch::QuadInstance quad = {
        mat4(), vec2(0,1-welcome_h), vec2(1,welcome_h)
    };
    quad_batch.instances.push_back(quad);
    quad_batch.texture_unit = TextureUnit::WELCOME_SCREEN;
    press_space_txt.lines.push_back("[Press SPACE]");
    credits_txt.lines.push_back("Melanie PAQUE");
    credits_txt.lines.push_back("Joya HADDAD");
    credits_txt.lines.push_back("Yoan LECOQ");
    const vec4 txt_rgba = vec4(23, 63, 96, 255.f)/255.f;
    press_space_txt.rgba = txt_rgba;
    credits_txt    .rgba = txt_rgba/2.f;
    credits_txt    .rgba.a = 1;
    press_space_txt.line_height = 0.07f;
    credits_txt    .line_height = 0.05f;

    reshape(viewport_size);
}

WelcomeScreen::~WelcomeScreen() {
    --refcount;
    if(!refcount)
        cleanupGL();
}

void WelcomeScreen::reshape(ivec2 new_viewport_size) {
    view.reshape(new_viewport_size);
    const float vhh = view.getHalfHeight();

    quad_batch.instances[0].modelmatrix = scale(vec3(2*vhh/welcome_h,2*vhh,1));
    quad_batch.updateInstancesVBO();

    press_space_txt.position = vec3(-press_space_txt.getLineWidth(0)/2.f, -.07f,0);
    press_space_txt.updateQuadBatch();

    credits_txt.position = vec3(0,0,0);
    credits_txt.position.x = -view.half_width;
    credits_txt.position.y = (credits_txt.lines.size()-1)*credits_txt.line_height - vhh;
    credits_txt.updateQuadBatch();
}

GameplayType WelcomeScreen::nextFrame(const Input &input, uint32_t fps) {
    if(input.interact)
        is_in_transition = true;

    if(is_in_transition) {
        bg_color.progress += 1.4f/fps;
        float progress = clamp<float>(bg_color.progress,0,1);
        quad_batch.rgb_fx_factor = progress;
        credits_txt.rgba.a = 1-progress;
        press_space_txt.rgba.a = 1-progress;
        press_space_txt.updateQuadBatch();
        credits_txt.updateQuadBatch();
    }

    vec3 c = bg_color.getCurrent();
    glClearColor(c.x, c.y, c.z, 1);

    return bg_color.progress>=1.6f ? GameplayType::WORLD_MAP : GameplayType::WELCOME_SCREEN;
}

void WelcomeScreen::renderGL() const {
    quad_batch.renderGL_HUD(view);
    credits_txt.renderGL_HUD(view);
    press_space_txt.renderGL_HUD(view);
}

} // namespace dm
