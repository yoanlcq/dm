#pragma once

#include <dm/dm.hpp>

namespace dm {

// Helper to count FPS and (if needed) limit the framerate.
struct FpsCounter {
    uint32_t fps_ceil;
    uint32_t framerate_limit;
    double   frame_count;
    uint32_t lim_last_time;
    uint32_t last_time;
    uint32_t fps; // This member is computed from the above ones.
    void waitBeforePresenting();
    void nextFrame();
};

}