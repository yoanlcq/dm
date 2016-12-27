#pragma once
#include <dm/dm.hpp>

namespace dm {

struct GLMonospaceText {
    GLQuadBatch   quad_batch;
    std::string text;
    float       text_size_viewspace;

    static GLuint    texture;
    static glm::vec2 atlas_char_size;

    static void setupGL();
    static void cleanupGL();
    void updateQuadBatch();
    void renderGL() const; // Disables GL_DEPTH_TEST then re-enables it.
};

} // namespace dm
