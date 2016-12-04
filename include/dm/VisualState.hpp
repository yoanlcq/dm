#pragma once
#include <dm/dm.hpp>

namespace dm {

// The VisualState struct contains every OpenGL object related to how we display
// the game. It is computed from the GameState, not the other way around.

struct QuadVertex {
    glm::vec2 position;
    glm::vec2 texcoords;
    QuadVertex(glm::vec2 p = glm::vec2(0,0), 
               glm::vec2 t = glm::vec2(0,0)) 
        : position(p), texcoords(t) {};
    ~QuadVertex() {}
    static const GLuint  ATTRIB_POSITION;
    static const GLuint  ATTRIB_TEXCOORDS;
    static const GLvoid* OFFSET_POSITION;
    static const GLvoid* OFFSET_TEXCOORDS;
};

struct QuadInstance {
    glm::ivec3 tilecoords; // Not ivec2: we want to be able to draw the floor and ceiling as well, which requires an extra dimension.
    glm::ivec3 normal;
    GLint      texindex;
    QuadInstance(
        glm::ivec3 tilecoords = glm::ivec3(0,0,0),
        glm::ivec3 normal     = glm::ivec3(0,0,0),
        GLint      texindex   = 0
    ) : tilecoords(tilecoords), normal(normal), texindex(texindex) {}
    ~QuadInstance() {}
    static const GLuint  ATTRIB_TILECOORDS;
    static const GLuint  ATTRIB_NORMAL;
    static const GLuint  ATTRIB_TEXINDEX;
    static const GLvoid* OFFSET_TILECOORDS;
    static const GLvoid* OFFSET_NORMAL;
    static const GLvoid* OFFSET_TEXINDEX;
};

struct QuadBatch {
    std::vector<QuadInstance> instances;
    GLuint instances_vbo, data_vbo, vao;
    QuadBatch();
    ~QuadBatch();
    void updateInstancesVBO() const;
    void renderGL() const;
};

// Must match res/shaders/quad.frag.
#define QUADPROGRAM_MAX_TEXTURES 8
struct QuadProgram {
    GLProgram prog;
    GLint u_viewprojmatrix;
    GLint u_textures[QUADPROGRAM_MAX_TEXTURES];
    QuadProgram();
    ~QuadProgram();
};

struct Camera {
    glm::vec3 position;
    float theta; // rotation around Y, in radians
    glm::ivec2 viewport_size;
    Camera(glm::ivec2 viewport_size);
    ~Camera();
    void reshape(glm::ivec2 new_viewport_size);
    glm::vec3 getFrontVector() const;
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getViewProjectionMatrix() const;
};

struct VisualState {
    uint64_t frame_count;
    uint32_t frames_per_second;
    
    Camera camera;
    GLTexture textures[QUADPROGRAM_MAX_TEXTURES];
    QuadProgram quad_program;
    QuadBatch quad_batch;

    VisualState(glm::ivec2 viewport_size);
    ~VisualState();
    void reshape(glm::ivec2 new_viewport_size);
    void update(const GameState &game_state);
    void renderGL() const;
};

}
