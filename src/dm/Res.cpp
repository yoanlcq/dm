#include <dm/dm.hpp>

using namespace std;
using namespace glm;

namespace dm {

const string Res::QUADBATCH_VERT     = "res/shaders/quad2.vert";
const string Res::QUADBATCH_FRAG     = "res/shaders/quad2.frag";
const string Res::WELCOME_SCREEN_PNG = "res/welcome_screen.png";
const string Res::WORLD_MAP_PNG      = "res/world_map.png";
const string Res::WORLD_MAP_TXT      = "res/world_map.txt";
const string Res::WORLD_MAP_HERO_PNG = "res/world_map_hero.png";

struct ScopedLoadingTextPrinter {
    ScopedLoadingTextPrinter(const string &s) {
        cout << "Loading `" << s << "'...";
        cout.flush();
    }
    ~ScopedLoadingTextPrinter() { cout << " done." << endl; }
};

bool Res::setup() {
    { 
        ScopedLoadingTextPrinter sltp(WELCOME_SCREEN_PNG);
        welcome_screen_tex = GLTexture_fromFile(WELCOME_SCREEN_PNG);
        hope(welcome_screen_tex);
    }
    return true;
}
void Res::cleanup() {
    glDeleteTextures(1, &welcome_screen_tex);
}

} // namespace dm
