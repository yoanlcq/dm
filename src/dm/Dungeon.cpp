#include <dm/dm.hpp>

using namespace std;
using namespace glm;

namespace dm {

size_t Dungeon::refcount(0);

void Dungeon::setupGL() {}
void Dungeon::cleanupGL() {}

Dungeon:: Dungeon(ivec2 viewport_size) : view(viewport_size) {
    if(!refcount)
        setupGL();
    ++refcount;
}
Dungeon::~Dungeon() {
    --refcount;
    if(!refcount)
        cleanupGL();
}
void Dungeon::reshape(ivec2 new_viewport_size) {
    view.reshape(new_viewport_size);
}
GameplayType Dungeon::nextFrame(const Input &input, uint32_t fps) {
    glClearColor(0,1,1,1);
    return GameplayType::DUNGEON;
}
void Dungeon::renderGL() const {}

} // namespace dm
