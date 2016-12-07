#include <dm/dm.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Si on se trouve dans le répertoire de l'exécutable,
// on ne trouvera pas les ressources - il faut se mettre
// dans son dossier parent.
// Cette solution n'est pas très propre, mais c'est peu
// probable qu'elle pose des problèmes.
static void fix_cwd(const char *argv0) {
#ifdef _WIN32
    const char *argv0_last = strrchr(argv0, '\\');
#else
    const char *argv0_last = strrchr(argv0, '/');
#endif
    argv0 = argv0_last ? argv0_last+1 : argv0;
#ifdef _WIN32
    BOOL success;
    if(GetFileAttributesA(argv0) != INVALID_FILE_ATTRIBUTES)
        success = SetCurrentDirectoryA("..");
    assert(success);
#else
    if(!access(argv0, F_OK))
        chdir("..");
#endif
}

#ifdef _WIN32
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
#ifdef _WIN32
    fix_cwd(__argv[0]);
#else
    fix_cwd(argv[0]);
#endif
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

