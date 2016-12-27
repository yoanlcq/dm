#pragma once

#include <dm/dm.hpp>

namespace dm {

struct WelcomeScreen {
    void updateState(const Input &input);
    void updateVisuals();
    void renderGL() const;
};

}
