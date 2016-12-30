#pragma once

#include <dm/dm.hpp>

namespace dm {

// Helper template for linear interpolations.
// First, set its 'prev' and 'next' values - then, every
// animation frame, just add somme value to 'progress',
// and get the current state with 'getCurrent()'.
template<typename T> 
struct Lerp {
    T prev, next;
    float progress;

    Lerp() { memset(this, 0, sizeof *this); }
    Lerp(T curr)
        : prev(curr), next(curr), progress(0) {}
    Lerp(T prev, T next, float progress=0) 
        : prev(prev), next(next), progress(progress) {}
    ~Lerp() {}

    void reset(T curr = T({})) {
        *this = Lerp(curr);
    }
    T getCurrent() const {
        float p = clamp<float>(progress, 0, 1);
        return prev*(1-p) + next*p;
    }
    operator T() const {
        return getCurrent();
    }
};


template<typename T> 
T lerp(T prev, T next, float progress) {
    return Lerp<T>(prev, next, progress).getCurrent();
}
static inline float lerpf(float prev, float next, float progress) {
    return lerp<float>(prev, next, progress);
}

} // namespace dm
