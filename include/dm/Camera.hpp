#pragma once
#include <dm/dm.hpp>

namespace dm {

struct Camera {
    Lerp<glm::vec3> position;
    Lerp<float>     angle_y;
    glm::ivec2      viewport_size;

    Camera(glm::ivec2 viewport_size);
    ~Camera();

    void reshape(glm::ivec2 new_viewport_size);
    glm::vec3 getFrontVector() const;
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getViewProjectionMatrix() const;
};

} // namespace dm
