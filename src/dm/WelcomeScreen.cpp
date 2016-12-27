#include <dm/dm.hpp>

namespace dm {

void WelcomeScreen::updateState(const Input &input) {
    glClearColor(abs(input.axis.x), abs(input.axis.y), .5, 1);
}
void WelcomeScreen::updateVisuals() {}
void WelcomeScreen::renderGL() const {}

} // namespace dm
