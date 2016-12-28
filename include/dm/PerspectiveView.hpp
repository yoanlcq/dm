#pragma once
#include <dm/dm.hpp>

namespace dm {

struct PerspectiveView {
    glm::ivec2      viewport_size;
    Lerp<glm::vec3> position;
    Lerp<float>     angle_y;
    Lerp<float>     fov_y;

    PerspectiveView(glm::ivec2 viewport_size);
    ~PerspectiveView();

    void reshape(glm::ivec2 new_viewport_size);
    glm::vec3 getFrontVector() const;
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getViewProjectionMatrix() const;
};

} // namespace dm
