#pragma once

#include <dm/dm.hpp>

namespace dm {

struct Game;

struct EnsureGLContext {
    EnsureGLContext(Game *g);
};

struct Game {
    bool                 should_quit;
    int                  exit_status;

    SDL_Window*          window;
    SDL_GLContext        gl_context;
    SDL_DisplayMode      current_display_mode;
    glm::ivec2           window_size, prev_window_size;
    EnsureGLContext      ensure_gl_context;
    FpsLimiter           fps_limiter;
    
    RawInput             raw_input;
    Input                input;

    GameplayType         gameplay, next_gameplay;

    WelcomeScreen        welcome_screen;
    WorldMap             world_map;
    Dungeon              dungeon; // A value copy from one of the dungeons.


    Game(glm::ivec2 window_size = glm::ivec2(800, 600));
    ~Game();

    void reshape(glm::ivec2 new_window_size);
    bool pollSDL2Event(SDL_Event &e) const;
    void handleSDL2Event(const SDL_Event &e);

    bool isFullscreen() const;
    void enterFullscreen();
    void leaveFullscreen();

    uint32_t getFPS() const;
    void nextFrame();
    void renderGL() const;
    void clearGL() const;
    void presentGL(); // Not const because it writes to fps_limiter.

    bool saveToFile(std::string path) const;

};

} // namespace dm
