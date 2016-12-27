#include <dm/dm.hpp>

using namespace std;
using namespace glm;


namespace dm {


// No use changing this.
static const uint32_t UPDATE_GAME_STATE_BIT = 0x20;

static uint32_t timer_callback(uint32_t interval, void *param) {
    SDL_Event e;
    e.type       = SDL_USEREVENT;
    e.user.type  = SDL_USEREVENT;
    e.user.code  = 0;
    e.user.data1 = param;
    e.user.data2 = NULL;
    SDL_PushEvent(&e);
    return interval;
}



Game::Game(glm::ivec2 window_size) 
    : window_size(window_size), 
      prev_window_size(window_size),
      camera(window_size)
{

    // Clearing everything to 0 is a good start.
    memset(this, 0, sizeof(*this));

    should_quit = true;
    exit_status = EXIT_FAILURE;

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cerr << "SDL_Init(): " << SDL_GetError() << endl;
        return;
    }

    int img_flags = IMG_INIT_JPG | IMG_INIT_PNG;
    int img_init  = IMG_Init(img_flags);
    if((img_init&img_flags) != img_flags) {
        cerr << "IMG_Init(): " << IMG_GetError() << endl;
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 
                        0 //SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG
#ifdef DM_GL_DEBUG
                       | SDL_GL_CONTEXT_DEBUG_FLAG
#endif
                        );
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, 
                        SDL_GL_CONTEXT_PROFILE_CORE
    );

    window = SDL_CreateWindow("Dungeon master", 
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
            window_size.x, window_size.y,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if(!window) {
        cerr << "SDL_CreateWindow(): " << SDL_GetError() << endl;
        return;
    }

    // Screen resolution for fullscreen mode.
    if(SDL_GetCurrentDisplayMode(0, &current_display_mode)) {
        cerr << "SDL_GetCurrentDisplayMode(): " << SDL_GetError() << endl;
        return;
    }

    /* Seems redundant, but actually creating an SDL2 window
     * does not imply GL context creation.
     * SDL2's docs states that you need to call SDL_GL_CreateContext() before
     * any GL calls. */
    gl_context = SDL_GL_CreateContext(window);
    if(!gl_context) {
        cerr << "SDL_GL_CreateContext(): " << SDL_GetError() << endl;
        return;
    }

    glewExperimental = GL_TRUE;
    cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;
    GLenum glew = glewInit();
    if(glew != GLEW_OK) {
        cerr << "Could not initialize GLEW : " 
             << glewGetErrorString(glew) << endl;
        return;
    }

    // Clear the last error code
    glGetError();


    GLint ctxflags, ctxpflags, gl_major, gl_minor;
    glGetIntegerv(GL_CONTEXT_FLAGS, &ctxflags);
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &ctxpflags);
    glGetIntegerv(GL_MAJOR_VERSION, &gl_major);
    glGetIntegerv(GL_MINOR_VERSION, &gl_minor);

    cout << "OpenGL Version  : " << glGetString(GL_VERSION)  << endl
         << "(glGetIntegerv) : " << gl_major << "." << gl_minor << endl
         << "OpenGL Renderer : " << glGetString(GL_RENDERER) << endl
         << "OpenGL Vendor   : " << glGetString(GL_VENDOR)   << endl
         << "GLSL Version    : " << glGetString(GL_SHADING_LANGUAGE_VERSION) 
         << endl
         << "Profile flags   : " 
         << (ctxpflags & GL_CONTEXT_CORE_PROFILE_BIT ? "core " : "") 
         << (ctxpflags & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT ? "compatibility " : "") 
         << hex << "(0x" << ctxpflags << ")"
         << endl
         << "Context flags   : " 
         << (ctxflags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT ? "forward-compatible " : "") 
         << (ctxflags & GL_CONTEXT_FLAG_DEBUG_BIT ? "debug " : "")
         << hex << "(0x" << ctxflags << ")"
         << dec << endl;

    gl_debug_setup(gl_major, gl_minor, 
#ifdef DM_GL_DEBUG
            true
#else
            false
#endif
    );

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(SDL_GL_SetSwapInterval(1) < 0)
        cerr << "Warning : Vsync is disabled. The FPS may skyrocket." << endl;


    fps_limiter.fps_ceil        = 128;
    fps_limiter.framerate_limit = 0;
    fps_limiter.frame_count     = 0;
    fps_limiter.lim_last_time   = SDL_GetTicks();
    fps_limiter.last_time       = SDL_GetTicks();

    hope(res.setup());
    GLQuadBatch::setupGL();

    gameplay = GAMEPLAY_WELCOME_SCREEN;
    updateState(); // Do it once here...
    //... before scheduling it.
    SDL_AddTimer(UPDATESTATE_DELAY_MS, timer_callback, (void*)UPDATE_GAME_STATE_BIT);

    should_quit = false;
    exit_status = EXIT_SUCCESS;
}

Game::~Game() {
    res.cleanup();
    GLQuadBatch::cleanupGL();
    IMG_Quit();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}



void Game::reshape(ivec2 size) {
    glViewport(0, 0, size.x, size.y);
    window_size = size;
    camera.reshape(size);
}

bool Game::pollSDL2Event(SDL_Event &e) const {
    return SDL_PollEvent(&e);
}

void Game::handleSDL2Event(const SDL_Event &event) {
    switch(event.type) {
    case SDL_USEREVENT:
        if((uint32_t)(uintptr_t)event.user.data1 & UPDATE_GAME_STATE_BIT)
            updateState(); 
        break;
    case SDL_QUIT: 
    case SDL_APP_TERMINATING: 
        should_quit = true; 
        break;
    case SDL_WINDOWEVENT: 
        switch (event.window.event) {
        case SDL_WINDOWEVENT_CLOSE:
            should_quit = true;
            break;
        case SDL_WINDOWEVENT_RESIZED:
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            reshape(ivec2(event.window.data1, event.window.data2));
            break;
        }
        break;
    }
    raw_input.handleSDL2Event(event);
}



bool Game::isFullscreen() const {
    return !!(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN);
}

void Game::enterFullscreen()  {
    prev_window_size = window_size;
    SDL_SetWindowSize(window, current_display_mode.w, current_display_mode.h);
    if(SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) < 0)
        cerr << "Failed to enter fullscreen : " << SDL_GetError() << endl;
}

void Game::leaveFullscreen()  {
    if(SDL_SetWindowFullscreen(window, false) < 0) {
        cerr << "Failed to leave fullscreen : " << SDL_GetError() << endl;
        return;
    }
    SDL_SetWindowSize(window, prev_window_size.x, prev_window_size.y);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, 
                                  SDL_WINDOWPOS_CENTERED);
}




// Keep it under the framerate (that is, should be > 33)
const uint32_t Game::UPDATESTATE_DELAY_MS = 50;

void Game::updateState() { 
    
    input.recomputeFromRawInput(raw_input);
    
    if(input.toggle_fullscreen) {
        if(isFullscreen())
            leaveFullscreen();
        else
            enterFullscreen();
    }
    input.toggle_fullscreen = false; // Just in case

    switch(gameplay) {
    case GAMEPLAY_WELCOME_SCREEN: welcome_screen.updateState(input); break;
    case GAMEPLAY_WORLD_MAP:      world_map     .updateState(input); break;
    case GAMEPLAY_DUNGEON:        dungeon       .updateState(input); break;
    }
    
    
    raw_input.clearClicked();
}

void Game::updateVisuals() {
    switch(gameplay) {
    case GAMEPLAY_WELCOME_SCREEN: welcome_screen.updateVisuals(); break;
    case GAMEPLAY_WORLD_MAP:      world_map     .updateVisuals(); break;
    case GAMEPLAY_DUNGEON:        dungeon       .updateVisuals(); break;
    }
}

void Game::renderGL() const { 
    switch(gameplay) {
    case GAMEPLAY_WELCOME_SCREEN: welcome_screen.renderGL(); break;
    case GAMEPLAY_WORLD_MAP:      world_map     .renderGL(); break;
    case GAMEPLAY_DUNGEON:        dungeon       .renderGL(); break;
    }
}

void Game::clearGL() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Game::presentGL() {
    if(fps_limiter.framerate_limit > 0)
        fps_limiter.waitBeforePresenting();
    fps_limiter.nextFrame();
    SDL_GL_SwapWindow(window);
}




bool Game::saveToFile(std::string path) const {
    assert("This function was not implemented yet !" && false);
    return false;
}





} // namespace dm
