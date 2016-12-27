#include <dm/dm.hpp>

using namespace std;
using namespace glm;

namespace dm {

Camera::Camera(ivec2 viewport_size) 
    : position(vec3(0,0,0)), 
      angle_y(M_PI/2.f), 
      viewport_size(viewport_size) 
    {}

Camera::~Camera() {}

void Camera::reshape(ivec2 new_viewport_size) {
    viewport_size = new_viewport_size;
}
vec3 Camera::getFrontVector() const {
    float theta = angle_y.getCurrent();
    return vec3(cosf(theta), 0, -sinf(theta));
}
mat4 Camera::getViewMatrix() const {
    vec3 pos = position.getCurrent();
    return lookAt(pos, pos+getFrontVector(), vec3(0,1,0));
}
mat4 Camera::getProjectionMatrix() const {
    return perspective(
        radians(60.f), viewport_size.x/(float)viewport_size.y, .01f, 100.f
    );
}
mat4 Camera::getViewProjectionMatrix() const {
    return getProjectionMatrix()*getViewMatrix();
}

} // namespace dm
