#pragma once
#include <dm/dm.hpp>

namespace dm {

struct InputKeys { 
    bool z, q, s, d, a, e;
    bool up, down, left, right;
    bool space, enter, plus, minus; 
    bool mouseleft, mousemiddle, mouseright;
    bool f11, esc;
};
struct RawInput { 
    InputKeys held, clicked;
    int mousescroll;
    glm::ivec2 mouseprevpos, mousepos;
    void clearClicked();
    void handleSDL2Event(const SDL_Event &e);
};
struct Input {
    glm::ivec2 axis;
    int turnaround;
    bool interact;
    bool escape;
    bool toggle_fullscreen;
    void recomputeFromRawInput(const RawInput&);
};

} // namespace dm
