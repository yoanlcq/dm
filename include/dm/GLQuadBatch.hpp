#pragma once

#include <dm/dm.hpp>

namespace dm {

struct GLQuadInstance {
    glm::vec2 vposition[4];
    glm::vec2 vtexcoords[4];
    uint32_t  texindex;
    glm::mat4 modelmatrix;
    uint32_t  is_hud;
    GLfloat   shadow_factor;
    GLfloat   fog_factor;
};

struct GLQuadBatch {    

    std::vector<GLQuadInstance> instances;
    GLuint vbo, vao;

    static constexpr size_t MAX_TEXTURES = 32; // Must match res/shaders/quad2.frag
    static GLuint texunits[MAX_TEXTURES];
    static GLuint prog;
    static GLint u_textures[MAX_TEXTURES];
    static GLint u_fogdistance;
    static GLint u_fogcolor;
    static GLint u_viewprojmatrix;
    static GLint u_viewposition;
    static constexpr GLuint  ATTRIB_POSITION      = 0;
    static constexpr GLuint  ATTRIB_TEXCOORDS     = 1;
    static constexpr GLuint  ATTRIB_TEXINDEX      = 2;
    static constexpr GLuint  ATTRIB_MODELMATRIX   = 3;
    static constexpr GLuint  ATTRIB_IS_HUD        = 7;
    static constexpr GLuint  ATTRIB_SHADOW_FACTOR = 8;
    static constexpr GLuint  ATTRIB_FOG_FACTOR    = 9;
    static constexpr GLvoid* OFFSET_POSITION      = (GLvoid*) offsetof(GLQuadInstance, vposition);
    static constexpr GLvoid* OFFSET_TEXCOORDS     = (GLvoid*) offsetof(GLQuadInstance, vtexcoords);
    static constexpr GLvoid* OFFSET_TEXINDEX      = (GLvoid*) offsetof(GLQuadInstance, texindex);
    static constexpr GLvoid* OFFSET_MODELMATRIX   = (GLvoid*) offsetof(GLQuadInstance, modelmatrix);
    static constexpr GLvoid* OFFSET_IS_HUD        = (GLvoid*) offsetof(GLQuadInstance, is_hud);
    static constexpr GLvoid* OFFSET_SHADOW_FACTOR = (GLvoid*) offsetof(GLQuadInstance, shadow_factor);
    static constexpr GLvoid* OFFSET_FOG_FACTOR    = (GLvoid*) offsetof(GLQuadInstance, fog_factor);

    // Call this once after OpenGL context creation.
    static void setupGL();
    static void cleanupGL();

    GLQuadBatch();
    ~GLQuadBatch();

    void updateInstancesVBO() const;
    void renderGL() const;
};

} // namespace dm
