#pragma once

#include <dm/dm.hpp>

namespace dm {

// Helper template for constraining values to a certain range.
// See also : GLSL's clamp().
template<typename T>
T clamp(T x, T minVal, T maxVal) {
    return std::min(maxVal, std::max(minVal, x));
}

// Helper template for linear interpolations.
// First, set its 'prev' and 'next' values - then, every
// animation frame, just add somme value to 'progress',
// and get the current state with 'getCurrent()'.
template<typename T> 
struct Lerp {
    T prev, next;
    float progress;

    Lerp(T curr=T({}))   : prev(curr), next(curr), progress(0) {}
    Lerp(T prev, T next) : prev(prev), next(next), progress(0) {}
    ~Lerp() {}

    T getCurrent() const {
        return prev*(1-progress) + next*progress;
    }
    bool isComplete() const {
        return progress >= .996f;
    }
};

} // namespace dm
