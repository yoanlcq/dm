#pragma once

#include <dm/dm.hpp>

namespace dm {

// Ensemble de données pour compter le nombre de Frames-Par-Seconde
// et les limiter si besoin.
struct FpsLimiter {
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
