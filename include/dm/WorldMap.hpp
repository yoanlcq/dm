#pragma once

#include <dm/dm.hpp>

namespace dm {

struct WorldMap {
    GLuint bg_tex;
    GLuint hero_tex;
    struct Node {
        DungeonIndex dungeon_index;
        glm::vec2 position;
        size_t right, up, left, down; 
    };
    typedef size_t NodeIndex;
    std::vector<Node> nodes;
    NodeIndex current_node;

    void updateState(const Input &input);
    void updateVisuals();
    void renderGL() const;
};

}
