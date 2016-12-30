#include <dm/dm.hpp>

using namespace std;
using namespace glm;

namespace dm {

PerspectiveView::PerspectiveView(ivec2 viewport_size) 
    : viewport_size(viewport_size),
      position(vec3(0,0,0)), 
      angle_y(M_PI/2.f), 
      fov_y(60.f)
    {}

PerspectiveView::~PerspectiveView() {}

void PerspectiveView::reshape(ivec2 new_viewport_size) {
    viewport_size = new_viewport_size;
}
vec3 PerspectiveView::getFrontVector() const {
    return vec3(cosf(angle_y), 0, -sinf(angle_y));
}
mat4 PerspectiveView::getViewMatrix() const {
    vec3 pos = position.getCurrent();
    return lookAt(pos, pos+getFrontVector(), vec3(0,1,0));
}
mat4 PerspectiveView::getProjectionMatrix() const {
    return perspective(
        radians(fov_y.getCurrent()), 
        viewport_size.x/(float)viewport_size.y, 
        .01f, 100.f
    );
}
mat4 PerspectiveView::getViewProjectionMatrix() const {
    return getProjectionMatrix()*getViewMatrix();
}

} // namespace dm
