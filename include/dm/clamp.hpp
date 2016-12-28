#pragma once

#include <dm/dm.hpp>

namespace dm {

// Helper template for constraining values to a certain range.
// See also : GLSL's clamp().
template<typename T>
T clamp(T x, T minVal, T maxVal) {
    return std::min(maxVal, std::max(minVal, x));
}

template<typename T>
void clamp_ref(T& x, T minVal, T maxVal) {
    x = clamp<T>(x, minVal, maxVal);
}


} // namespace dm
