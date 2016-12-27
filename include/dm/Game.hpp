#pragma once

#include <dm/dm.hpp>

namespace dm {

struct Game {
    bool                 should_quit;
    int                  exit_status;

    SDL_Window*          window;
    SDL_GLContext        gl_context;
    SDL_DisplayMode      current_display_mode;
    glm::ivec2           window_size, prev_window_size;
    FpsLimiter           fps_limiter;
    
    Res                  res; // resources
    RawInput             raw_input;
    Input                input;

    Camera               camera;

    // For lack of a better name...
    // We're either in the welcome screen, or on the world map, or in a dungeon.
    // Concerns differ while updating, rendering, and stuff.
    enum Gameplay {
        GAMEPLAY_WELCOME_SCREEN = 0, // Leave it to 0
        GAMEPLAY_WORLD_MAP,
        GAMEPLAY_DUNGEON
    };
    Gameplay             gameplay;

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

    static const uint32_t UPDATESTATE_DELAY_MS; // Fixed delay between calls to updateState()
    // The three functions below change their behaviour depending on the 'gameplay' member's value.
    void updateState();
    void updateVisuals();
    void renderGL() const;
    void clearGL() const;
    void presentGL(); // Not const because it writes to fps_limiter.

    bool saveToFile(std::string path) const;
};

} // namespace dm
