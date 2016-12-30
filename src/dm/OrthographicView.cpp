#include <dm/dm.hpp>

using namespace std;
using namespace glm;

namespace dm {

OrthographicView::OrthographicView(ivec2 viewport_size) 
    : viewport_size(viewport_size),
      half_width(1),
      position(vec2(0,0)),
      zoom(1)
    {}

OrthographicView::~OrthographicView() {}

void OrthographicView::reshape(ivec2 new_viewport_size) {
    viewport_size = new_viewport_size;
}
float OrthographicView::getAspectRatio() const {
    return viewport_size.x/float(viewport_size.y);
}
float OrthographicView::getHalfHeight() const {
    return half_width/getAspectRatio();
}
mat4 OrthographicView::getViewMatrix() const {
    return translate(scale(vec3(zoom,zoom,zoom)), -vec3(position.getCurrent(), 0));
}
mat4 OrthographicView::getProjectionMatrix() const {
    return ortho(-half_width, half_width, -getHalfHeight(), getHalfHeight());
}
mat4 OrthographicView::getViewProjectionMatrix() const {
    return getProjectionMatrix()*getViewMatrix();
}

} // namespace dm
