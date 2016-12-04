#pragma once

namespace dm {

struct Input {
    bool  mouse_in; // Is the mouse within the window ?
    glm::ivec2 mouse, mousemove;
    glm::ivec2 move, move_held;// Values are either -1 or 1.
    int32_t rotate, rotate_held;
    bool attack, attack_held;
};


}
