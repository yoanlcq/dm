#pragma once
#include <dm/dm.hpp>

namespace dm {

struct GLText {
    GLQuadBatch quad_batch;
    std::vector<std::string> lines;
    glm::vec3   position; // Bottom-left of 1st line's 1st character.
    float       height; // In world coords.
    glm::vec4   rgba;

    static GLuint    tex;

    GLText();
    ~GLText();

    float getLineWidth(size_t i) const;
    void updateQuadBatch();
    void renderGL_HUD(const OrthographicView &view) const;

private:
    static size_t refcount;
    static void setupGL();
    static void cleanupGL();
};

} // namespace dm
