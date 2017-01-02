#pragma once

#include <dm/dm.hpp>

namespace dm {

struct WelcomeScreen {
    OrthographicView view;
    Lerp<glm::vec3>  bg_color;
    static GLuint    tex;
    GLQuadBatch      quad_batch;
    bool             is_in_transition;
    GLText           press_space_txt;
    GLText           credits_txt;

     WelcomeScreen(glm::ivec2 viewport_size);
    ~WelcomeScreen();

    void reshape(glm::ivec2 new_viewport_size);
    GameplayType nextFrame(const Input &input, uint32_t fps);
    void renderGL() const;

private:
    static size_t refcount;
    static void setupGL();
    static void cleanupGL();
};

}
