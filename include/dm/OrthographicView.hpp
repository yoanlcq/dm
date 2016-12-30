#pragma once
#include <dm/dm.hpp>

namespace dm {

struct OrthographicView {
    glm::ivec2      viewport_size;
    float           half_width;
    Lerp<glm::vec2> position;
    float           zoom;

    OrthographicView(glm::ivec2 viewport_size);
    ~OrthographicView();

    void reshape(glm::ivec2 new_viewport_size);
    float getAspectRatio() const;
    float getHalfHeight() const;
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getViewProjectionMatrix() const;
};

} // namespace dm
