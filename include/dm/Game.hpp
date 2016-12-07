#pragma once

#include <dm/dm.hpp>

namespace dm {

struct Game;

// Auto-explicatif. On a besoin d'initialiser un contexte
// OpenGL avant toute chose.
struct EnsureGLContext {
    EnsureGLContext(Game &g);
    ~EnsureGLContext();
};

struct Game {
    bool should_quit;
    int exit_code;
    glm::ivec2 window_size, prev_window_size;
    EnsureGLContext ensure_gl_context;
    SDL_Window *window;
    SDL_DisplayMode current_display_mode;
    SDL_GLContext gl_context; 
    FpsCounter fps_counter;
    Input input;
    GameState game_state;
    VisualState visual_state;
    
    Game(glm::ivec2 window_size=glm::ivec2(600,400));
    ~Game();
    void reshape(const glm::ivec2 &size); // Appeler quand la taille du canevas change.
    bool isFullscreen() const;
    void enterFullscreen();
    void leaveFullscreen();
    bool pollSDL2Event(SDL_Event &e) const;
    void handleSDL2Event(const SDL_Event &e);
    void clearGL() const;
    void renderGL() const;
    void presentGL(); // swapBuffers().
    void updateVisualState();
    void updateGameState();
};

}
