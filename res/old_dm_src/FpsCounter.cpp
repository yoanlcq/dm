#include <dm/dm.hpp>

using namespace std;

namespace dm {

void FpsCounter::nextFrame() {
    // Should be in [100, 1000]
    static const double interval = 1000.0; 

    /* See http://www.opengl-tutorial.org/miscellaneous/an-fps-counter/ */
    uint32_t current_time = SDL_GetTicks();
    ++frame_count;
    if(SDL_TICKS_PASSED(current_time, last_time+interval))
    {
        fps = lround(frame_count*1000.0/interval);
        // cout << (unsigned)frame_count << " frames under "
        //      << interval << " milliseconds"
        //      << " = " << interval/frame_count << "milliseconds/frame"
        //      << " = " << fps << "FPS";
        frame_count = 0;
        last_time += interval;
        if(framerate_limit <= 0 && fps > fps_ceil) {
            framerate_limit = 60;
            cout << "Abnormal FPS detected (Vsync is not working). "
                 << "Now limiting FPS to " << framerate_limit << "." << endl;
        }
    }
}

void FpsCounter::waitBeforePresenting() {
    uint32_t current_time = SDL_GetTicks();
    uint32_t a_frame = 1000/framerate_limit;
    uint32_t diff = current_time - lim_last_time;

    if(a_frame > diff)
        SDL_Delay(a_frame - diff);

    lim_last_time = SDL_GetTicks();
}

}
