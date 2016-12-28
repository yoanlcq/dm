#pragma once

#include <dm/dm.hpp>

namespace dm {

struct GLQuadBatch {
    struct QuadVertex {
        glm::vec2 position;
        glm::vec2 texcoords;
    };
    struct QuadInstance {
        glm::mat4 modelmatrix;
        glm::vec2 sprite_pos;  // Bottom-left of sprite, in texture space
        glm::vec2 sprite_size; // Width-Height of sprite, in texture space
    };

    std::vector<QuadInstance> instances;
    TextureUnit               texture_unit;  // The texture unit used for every quad of this batch.
    GLfloat                   shadow_factor; // How much they are shadowed : From 0 to 1.
    GLfloat                   fog_factor;    // How much the fog affects them : From 0 to 1.
    GLfloat                   fogdistance;   // Distance at which objects are fully within the fog.
    glm::vec3                 fogcolor;      // Duh !
    glm::vec4                 rgba_fx;       // A color to mix with the quad's texture
    GLfloat                   rgba_fx_factor; // How much said color is used.

    GLQuadBatch();
    ~GLQuadBatch();

    void updateInstancesVBO() const;
    void renderGL(const PerspectiveView &view) const;
    void renderGL_HUD(const OrthographicView &view) const;

private:
    static size_t refcount;
    static void setupGL();
    static void cleanupGL();

    void renderGL_priv(const glm::mat4 &viewprojmatrix, const glm::vec3 &viewpos) const;
    GLuint vbo, vao;
    static GLuint static_vbo;
    static GLuint prog;
    static GLint u_viewprojmatrix_loc;
    static GLint u_viewposition_loc;
    static GLint u_texture_loc;
    static GLint u_shadow_factor_loc;
    static GLint u_fog_factor_loc;
    static GLint u_fogdistance_loc;
    static GLint u_fogcolor_loc;
    static GLint u_rgba_fx_loc;
    static GLint u_rgba_fx_factor_loc;
    static const GLuint   ATTRIB_POSITION;
    static const GLuint   ATTRIB_TEXCOORDS;
    static const GLuint   ATTRIB_MODELMATRIX;
    static const GLuint   ATTRIB_SPRITE_POS;
    static const GLuint   ATTRIB_SPRITE_SIZE;
    static const GLubyte* OFFSET_POSITION;
    static const GLubyte* OFFSET_TEXCOORDS;
    static const GLubyte* OFFSET_MODELMATRIX;
    static const GLubyte* OFFSET_SPRITE_POS;
    static const GLubyte* OFFSET_SPRITE_SIZE;
};

} // namespace dm
