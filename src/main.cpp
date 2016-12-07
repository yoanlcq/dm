#include <dm/dm.hpp>

#ifdef _WIN32
#include <windows.h>
int CALLBACK WinMain(
  _In_ HINSTANCE hInstance,
  _In_ HINSTANCE hPrevInstance,
  _In_ LPSTR     lpCmdLine,
  _In_ int       nCmdShow
)
#else
int main(int argc, char *argv[]) 
#endif
{
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

