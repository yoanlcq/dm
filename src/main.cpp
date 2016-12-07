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
    argv0 = strrchr(argv0, '\\');
#else
    argv0 = strrchr(argv0, '/');
#endif
    if(!argv0)
        return;
    ++argv0;
#ifdef _WIN32
    if(GetFileAttributesA(argv0) != INVALID_FILE_ATTRIBUTES)
        SetCurrentDirectoryA("..");
#else
    if(!access(argv0, F_OK))
        chdir("..");
#endif
}

int main(int argc, char *argv[]) {
    fix_cwd(argv[0]);
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

