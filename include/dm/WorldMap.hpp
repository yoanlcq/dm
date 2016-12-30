#pragma once

#include <dm/dm.hpp>

namespace dm {

struct WorldMap {

    static constexpr size_t NODE_COUNT = 5;

    OrthographicView view;
    Lerp<float>      fader_opacity;
    GLQuadBatch      bg_quad_batch;
    GLQuadBatch      hero_quad_batch;
    GLQuadBatch      fader_quad_batch;
    GLText           node_text[NODE_COUNT];
    size_t           current_node_index;
    size_t           next_node_index;
    Lerp<glm::vec3>  hero_position;
    bool             is_starting_dungeon;

    static GLuint tex;
    static GLuint fader_tex;

     WorldMap(glm::ivec2 viewport_size);
    ~WorldMap();

    void reshape(glm::ivec2 new_viewport_size);
    GameplayType nextFrame(const Input &input, uint32_t fps);
    void renderGL() const;

private:
    static size_t refcount;
    static void setupGL();
    static void cleanupGL();
};

}
