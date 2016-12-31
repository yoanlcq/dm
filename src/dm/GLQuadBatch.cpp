#include <dm/dm.hpp>

using namespace std;
using namespace glm;

namespace dm {

GLQuadBatch::QuadInstance::QuadInstance(mat4 mdl, vec2 sprpos, vec2 sprsize) 
    : modelmatrix(mdl), 
      sprite_pos(sprpos), 
      sprite_size(sprsize)
    {}

size_t GLQuadBatch::refcount(0);
GLuint GLQuadBatch::static_vbo(0);
GLuint GLQuadBatch::prog(0);
GLint  GLQuadBatch::u_viewprojmatrix_loc  (-1);
GLint  GLQuadBatch::u_viewposition_loc    (-1);
GLint  GLQuadBatch::u_texture_loc         (-1);
GLint  GLQuadBatch::u_shadow_factor_loc   (-1);
GLint  GLQuadBatch::u_fog_factor_loc      (-1);
GLint  GLQuadBatch::u_fogdistance_loc     (-1);
GLint  GLQuadBatch::u_fogcolor_loc        (-1);
GLint  GLQuadBatch::u_rgba_fx_loc         (-1);
GLint  GLQuadBatch::u_rgb_fx_factor_loc   (-1);
GLint  GLQuadBatch::u_alpha_fx_factor_loc (-1);
GLint  GLQuadBatch::u_src_alpha_factor_loc(-1);
const GLuint   GLQuadBatch::ATTRIB_POSITION   (0);
const GLuint   GLQuadBatch::ATTRIB_TEXCOORDS  (1);
const GLuint   GLQuadBatch::ATTRIB_MODELMATRIX(2);
const GLuint   GLQuadBatch::ATTRIB_SPRITE_POS (6);
const GLuint   GLQuadBatch::ATTRIB_SPRITE_SIZE(7);
const GLubyte* GLQuadBatch::OFFSET_POSITION   ((GLubyte*) offsetof(QuadVertex, position));
const GLubyte* GLQuadBatch::OFFSET_TEXCOORDS  ((GLubyte*) offsetof(QuadVertex, texcoords));
const GLubyte* GLQuadBatch::OFFSET_MODELMATRIX((GLubyte*) offsetof(QuadInstance, modelmatrix));
const GLubyte* GLQuadBatch::OFFSET_SPRITE_POS ((GLubyte*) offsetof(QuadInstance, sprite_pos));
const GLubyte* GLQuadBatch::OFFSET_SPRITE_SIZE((GLubyte*) offsetof(QuadInstance, sprite_size));


void GLQuadBatch::setupGL() {
    hope(prog = GLProgram_fromFiles(
        "res/shaders/QuadBatch.vert", 
        "res/shaders/QuadBatch.frag"
    ));

    u_viewprojmatrix_loc   = glGetUniformLocation(prog, "u_viewprojmatrix");
    u_viewposition_loc     = glGetUniformLocation(prog, "u_viewposition");
    u_texture_loc          = glGetUniformLocation(prog, "u_texture");
    u_shadow_factor_loc    = glGetUniformLocation(prog, "u_shadow_factor");
    u_fog_factor_loc       = glGetUniformLocation(prog, "u_fog_factor");
    u_fogdistance_loc      = glGetUniformLocation(prog, "u_fogdistance");
    u_fogcolor_loc         = glGetUniformLocation(prog, "u_fogcolor");
    u_rgba_fx_loc          = glGetUniformLocation(prog, "u_rgba_fx");
    u_rgb_fx_factor_loc    = glGetUniformLocation(prog, "u_rgb_fx_factor");
    u_alpha_fx_factor_loc  = glGetUniformLocation(prog, "u_alpha_fx_factor");
    u_src_alpha_factor_loc = glGetUniformLocation(prog, "u_src_alpha_factor");
    assert(u_viewprojmatrix_loc   != -1);
    assert(u_viewposition_loc     != -1);
    assert(u_texture_loc          != -1);
    assert(u_shadow_factor_loc    != -1);
    assert(u_fog_factor_loc       != -1);
    assert(u_fogdistance_loc      != -1);
    assert(u_fogcolor_loc         != -1);
    assert(u_rgba_fx_loc          != -1);
    assert(u_rgb_fx_factor_loc    != -1);
    assert(u_alpha_fx_factor_loc  != -1);
    assert(u_src_alpha_factor_loc != -1);

    glGenBuffers(1, &static_vbo);
    hope(static_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, static_vbo);
    QuadVertex vdata[4] = {
        { vec2(-.5f, -.5f), vec2(0,0) },
        { vec2(-.5f,  .5f), vec2(0,1) },
        { vec2( .5f,  .5f), vec2(1,1) },
        { vec2( .5f, -.5f), vec2(1,0) }
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof vdata, vdata, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLQuadBatch::cleanupGL() {
    glDeleteProgram(prog);
    glDeleteBuffers(1, &static_vbo);
}


GLQuadBatch:: GLQuadBatch() {
    if(!refcount)
        setupGL();
    ++refcount;

    assert(static_vbo);
    glGenVertexArrays(1, &vao);
    hope(vao);
    glGenBuffers(1, &vbo);
    hope(vbo);

    texture_unit     = TextureUnit::INVALID_UNIT;
    shadow_factor    = 0;
    fog_factor       = 0;
    fogdistance      = 0.001;
    fogcolor         = vec3(0,0,0);
    rgba_fx          = vec4(0,0,0,1);
    rgb_fx_factor    = 0;
    alpha_fx_factor  = 0;
    src_alpha_factor = 1;


    glBindVertexArray(vao);

    glEnableVertexAttribArray(ATTRIB_POSITION     );
    glEnableVertexAttribArray(ATTRIB_TEXCOORDS    );
    glEnableVertexAttribArray(ATTRIB_MODELMATRIX+0);
    glEnableVertexAttribArray(ATTRIB_MODELMATRIX+1);
    glEnableVertexAttribArray(ATTRIB_MODELMATRIX+2);
    glEnableVertexAttribArray(ATTRIB_MODELMATRIX+3);
    glEnableVertexAttribArray(ATTRIB_SPRITE_POS   );
    glEnableVertexAttribArray(ATTRIB_SPRITE_SIZE  );
    glVertexAttribDivisor(ATTRIB_POSITION     , 0);
    glVertexAttribDivisor(ATTRIB_TEXCOORDS    , 0);
    glVertexAttribDivisor(ATTRIB_MODELMATRIX+0, 1);
    glVertexAttribDivisor(ATTRIB_MODELMATRIX+1, 1);
    glVertexAttribDivisor(ATTRIB_MODELMATRIX+2, 1);
    glVertexAttribDivisor(ATTRIB_MODELMATRIX+3, 1);
    glVertexAttribDivisor(ATTRIB_SPRITE_POS   , 1);
    glVertexAttribDivisor(ATTRIB_SPRITE_SIZE  , 1);

    glBindBuffer(GL_ARRAY_BUFFER, static_vbo);
    glVertexAttribPointer(ATTRIB_POSITION , 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), OFFSET_POSITION );
    glVertexAttribPointer(ATTRIB_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), OFFSET_TEXCOORDS);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // This works only because GLM's matrices are column-major !
    glVertexAttribPointer(ATTRIB_MODELMATRIX+0, 4, GL_FLOAT, GL_FALSE, sizeof(QuadInstance), OFFSET_MODELMATRIX + 0*sizeof(vec4));
    glVertexAttribPointer(ATTRIB_MODELMATRIX+1, 4, GL_FLOAT, GL_FALSE, sizeof(QuadInstance), OFFSET_MODELMATRIX + 1*sizeof(vec4));
    glVertexAttribPointer(ATTRIB_MODELMATRIX+2, 4, GL_FLOAT, GL_FALSE, sizeof(QuadInstance), OFFSET_MODELMATRIX + 2*sizeof(vec4));
    glVertexAttribPointer(ATTRIB_MODELMATRIX+3, 4, GL_FLOAT, GL_FALSE, sizeof(QuadInstance), OFFSET_MODELMATRIX + 3*sizeof(vec4));
    glVertexAttribPointer(ATTRIB_SPRITE_POS   , 2, GL_FLOAT, GL_FALSE, sizeof(QuadInstance), OFFSET_SPRITE_POS );
    glVertexAttribPointer(ATTRIB_SPRITE_SIZE  , 2, GL_FLOAT, GL_FALSE, sizeof(QuadInstance), OFFSET_SPRITE_SIZE);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLQuadBatch::~GLQuadBatch() {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    --refcount;
    if(!refcount)
        cleanupGL();
}

struct DepthComparer {
    mat4 viewmatrix;
    DepthComparer(mat4 viewmatrix) : viewmatrix(viewmatrix) {}
    bool operator()(
        const GLQuadBatch::QuadInstance &lhs,
        const GLQuadBatch::QuadInstance &rhs) const 
    {
        const vec4 lp = viewmatrix * lhs.modelmatrix * vec4(0,0,0,1);
        const vec4 rp = viewmatrix * rhs.modelmatrix * vec4(0,0,0,1);
        return lp.z < rp.z;
    }
};
struct IndexDepthComparer {
    const vector<GLQuadBatch::QuadInstance> &instances;
    mat4 viewmatrix;
    IndexDepthComparer(
        const vector<GLQuadBatch::QuadInstance> &instances, 
        mat4 viewmatrix
    ) : instances(instances), viewmatrix(viewmatrix) {}
    bool operator()(size_t i1, size_t i2) const 
    {
        return DepthComparer(viewmatrix)(instances[i1], instances[i2]);
    }
};

void GLQuadBatch::sortInstancesByDepth(const mat4 &viewmatrix) {
    std::stable_sort(instances.begin(), instances.end(), 
        DepthComparer(viewmatrix)
    );
}

vector<size_t> GLQuadBatch::sortInstancesByDepthKeepingIndices(const mat4 &viewmatrix) {

    vector<size_t> idx(instances.size());
    std::iota(idx.begin(), idx.end(), 0);
    std::stable_sort(idx.begin(), idx.end(),
        IndexDepthComparer(instances, viewmatrix)
    );
    sortInstancesByDepth(viewmatrix);
    return idx;
}

void GLQuadBatch::updateInstancesVBO() const {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 
        instances.size()*sizeof instances[0],
        &instances[0], 
        GL_STREAM_DRAW // Assume it'll be updated a few times
    );
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLQuadBatch::renderGL(const PerspectiveView &view) const {
    renderGL_priv(
        view.getViewProjectionMatrix(), 
        view.position.getCurrent()
    );
}

void GLQuadBatch::renderGL_HUD(const OrthographicView &view) const {
    assert(glIsEnabled(GL_DEPTH_TEST));
    glDisable(GL_DEPTH_TEST);
    renderGL_priv(
        view.getViewProjectionMatrix(), 
        vec3(view.position.getCurrent(), 0)
    );
    glEnable(GL_DEPTH_TEST);
}

void GLQuadBatch::renderGL_priv(
    const mat4 &viewprojmatrix, 
    const vec3 &viewpos) const 
{
    glUseProgram(prog);

    assert(shadow_factor>=0 && shadow_factor<=1);
    assert(fog_factor>=0 && fog_factor<=1);
    assert(fogdistance > 0 && "fogdistance can't be zero, the fragment shader relies on it.");
    assert(rgb_fx_factor>=0 && rgb_fx_factor<=1);
    assert(alpha_fx_factor>=0 && alpha_fx_factor<=1);
    assert(src_alpha_factor>=0 && src_alpha_factor<=1);

    glUniformMatrix4fv(u_viewprojmatrix_loc, 1, GL_FALSE, value_ptr(viewprojmatrix));
    glUniform3fv(u_viewposition_loc, 1, value_ptr(viewpos));
    glUniform1i(u_texture_loc, size_t(texture_unit));
    glUniform1f(u_shadow_factor_loc, shadow_factor);
    glUniform1f(u_fog_factor_loc, fog_factor);
    glUniform1f(u_fogdistance_loc, fogdistance);
    glUniform3fv(u_fogcolor_loc, 1, value_ptr(fogcolor));
    glUniform4fv(u_rgba_fx_loc, 1, value_ptr(rgba_fx));
    glUniform1f(u_rgb_fx_factor_loc, rgb_fx_factor);
    glUniform1f(u_alpha_fx_factor_loc, alpha_fx_factor);
    glUniform1f(u_src_alpha_factor_loc, src_alpha_factor);
    
    glBindVertexArray(vao);
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, instances.size());
    glBindVertexArray(0);

    glUseProgram(0);
}

} // namespace dm

