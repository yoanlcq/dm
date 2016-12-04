#include <dm/dm.hpp>

int main(int argc, char *argv[]) {
    dm::Game g;
    while(!g.should_quit) {
        SDL_Event e;
        while(g.pollSDL2Event(e))
            g.handleSDL2Event(e);
        g.updateVisualState();
        g.clearGL();
        g.renderGL();
        g.presentGL();
    }
    return g.exit_code;
}

