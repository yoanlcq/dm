#include <dm/dm.hpp>

using namespace std;
using namespace glm;

namespace dm {

Character::Character() {
    memset(this, 0, sizeof *this);
    prev_angle_deg = angle_deg = 90;
}
Character::~Character() {}

void Character::move(ivec2 dir) {
    is_moving = true;
    vec2 rotated_dir = glm::rotate(vec2(dir), radians(float(angle_deg-90)));
    moving_direction.x = roundf(rotated_dir.x);
    moving_direction.y = roundf(rotated_dir.y);
    prev_moving_transition = 0;
    moving_transition %= 100;
}
void Character::attack() {
    is_attacking = true;
}
void Character::rotate(int32_t dir) {
    is_rotating = true;
    angle_direction = dir;
}
const uint32_t Character::MOVING_TRANSITION_STEP = 20;
void Character::update() {
    prev_angle_deg = angle_deg;
    if(is_rotating) {
        angle_deg += 360 + 15*angle_direction;
        angle_deg %= 360;
        if(angle_deg%90 == 0)
            is_rotating = false;
    }
    if(is_moving) {
        prev_moving_transition = moving_transition;
        moving_transition += MOVING_TRANSITION_STEP;
        moving_transition %= 100;
        if(moving_transition == 0) {
            position += moving_direction;
            moving_direction = ivec2(0,0);
            is_moving = false;
        }
    }
    if(is_attacking) {
    
    }

}

void GameState::update(Input &input) {
    ++tick_count;
    //cout << "Tick : " << tick_count << endl;
    prev_time_ms = time_ms;
    time_ms = SDL_GetTicks();

    if(!hero.is_moving && !hero.is_rotating) {
        if(input.move_held != ivec2(0,0))
            hero.move(input.move_held);
        else if(input.move != ivec2(0,0))
            hero.move(input.move);
    }
    if(!hero.is_moving) {
        if(input.rotate_held)
            hero.rotate(input.rotate_held);
        else if(input.rotate)
            hero.rotate(input.rotate);
    }
    if((input.attack || input.attack_held) && !hero.is_attacking 
    && !hero.is_rotating)
        hero.attack();
    input.move = ivec2(0,0);
    input.rotate = 0;
    input.attack = false;

    hero.update();

    if(!hero.is_moving)
        if(input.move_held != ivec2(0,0))
            hero.move(input.move_held);

}

}
