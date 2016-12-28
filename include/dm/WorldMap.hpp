#pragma once

#include <dm/dm.hpp>

namespace dm {

struct WorldMap {
    OrthographicView view;
    Lerp<float>   fader_opacity;
    GLQuadBatch   bg_quad_batch;
    GLQuadBatch   hero_quad_batch;
    GLQuadBatch   fader_quad_batch;
    static GLuint bg_tex;
    static GLuint hero_tex;
    static GLuint fader_tex;
    struct Node {
        DungeonIndex dungeon_index;
        glm::vec2 position;
        size_t right, up, left, down; 
    };
    typedef size_t NodeIndex;
    std::vector<Node> nodes;
    NodeIndex current_node;

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
