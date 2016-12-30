#include <dm/dm.hpp>

using namespace glm;

namespace dm {

void RawInput::clearClicked() {
    memset(&clicked, 0, sizeof clicked);
    mousescroll = 0;
    mouseprevpos = mousepos;
}

void RawInput::handleSDL2Event(const SDL_Event &e) {
#define HANDLE_KEY_EVT(is_down) \
        switch(e.key.keysym.sym) { \
        case SDLK_RETURN: \
        case SDLK_RETURN2: \
        case SDLK_KP_ENTER: \
                            held.enter = is_down; if(is_down) clicked.enter = true; break; \
        case SDLK_RIGHT:    held.right = is_down; if(is_down) clicked.right = true; break; \
        case SDLK_UP:       held.up    = is_down; if(is_down) clicked.up    = true; break; \
        case SDLK_LEFT:     held.left  = is_down; if(is_down) clicked.left  = true; break; \
        case SDLK_DOWN:     held.down  = is_down; if(is_down) clicked.down  = true; break; \
        case SDLK_PLUS: \
        case SDLK_KP_PLUS:  held.plus  = is_down; if(is_down) clicked.plus  = true; break; \
        case SDLK_MINUS: \
        case SDLK_KP_MINUS: held.minus = is_down; if(is_down) clicked.minus = true; break; \
        case SDLK_z:        held.z     = is_down; if(is_down) clicked.z     = true; break; \
        case SDLK_q:        held.q     = is_down; if(is_down) clicked.q     = true; break; \
        case SDLK_s:        held.s     = is_down; if(is_down) clicked.s     = true; break; \
        case SDLK_d:        held.d     = is_down; if(is_down) clicked.d     = true; break; \
        case SDLK_a:        held.a     = is_down; if(is_down) clicked.a     = true; break; \
        case SDLK_e:        held.e     = is_down; if(is_down) clicked.e     = true; break; \
        case SDLK_SPACE:    held.space = is_down; if(is_down) clicked.space = true; break; \
        case SDLK_F11:      held.f11   = is_down; if(is_down) clicked.f11   = true; break; \
        case SDLK_ESCAPE:   held.esc   = is_down; if(is_down) clicked.esc   = true; break; \
        }
#define HANDLE_MBT_EVT(is_down) \
        switch(e.button.button) { \
        case SDL_BUTTON_LEFT:   held.mouseleft   = is_down; if(is_down) clicked.mouseleft   = true; break; \
        case SDL_BUTTON_MIDDLE: held.mousemiddle = is_down; if(is_down) clicked.mousemiddle = true; break; \
        case SDL_BUTTON_RIGHT:  held.mouseright  = is_down; if(is_down) clicked.mouseright  = true; break; \
        }
    switch(e.type) {
    case SDL_KEYDOWN:         if(e.key.repeat) break; HANDLE_KEY_EVT(true);      break;
    case SDL_KEYUP:           if(e.key.repeat) break; HANDLE_KEY_EVT(false);     break;
    case SDL_MOUSEBUTTONDOWN: HANDLE_MBT_EVT(true);      break;
    case SDL_MOUSEBUTTONUP:   HANDLE_MBT_EVT(false);     break;
    case SDL_MOUSEWHEEL:      mousescroll += e.wheel.y;  break;
    case SDL_MOUSEMOTION:     mousepos = ivec2(e.motion.x, e.motion.y); break;
    }
#undef HANDLE_KEY_EVT
#undef HANDLE_MBT_EVT
}

void Input::recomputeFromRawInput(const RawInput& raw) {

    memset(this, 0, sizeof*this);

    if(raw.held.a)
        turnaround += 1;
    if(raw.held.e)
        turnaround -= 1;
    if(raw.held.up    || raw.held.z)
        axis.y += 1;
    if(raw.held.down  || raw.held.s)
        axis.y -= 1;
    if(raw.held.left  || raw.held.q)
        axis.x -= 1;
    if(raw.held.right || raw.held.d)
        axis.x += 1;

    if(raw.held.space 
    || raw.held.enter
    || raw.held.mouseleft)
        interact = true;

    if(raw.held.esc)
        escape = true;

    if(raw.clicked.f11)
        toggle_fullscreen = true;
}

} // namespace dm
