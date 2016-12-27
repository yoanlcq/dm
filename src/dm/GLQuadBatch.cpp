#include <dm/dm.hpp>

using namespace std;
using namespace glm;

namespace dm {

GLuint GLQuadBatch::texunits[MAX_TEXTURES];
GLuint GLQuadBatch::prog(0);
GLint  GLQuadBatch::u_textures[MAX_TEXTURES];
GLint  GLQuadBatch::u_fogdistance(-1);
GLint  GLQuadBatch::u_fogcolor(-1);
GLint  GLQuadBatch::u_viewprojmatrix(-1);
GLint  GLQuadBatch::u_viewposition(-1);

void GLQuadBatch::setupGL() {
    cout << "Compiling `" << Res::QUADBATCH_VERT 
         << "' and `"     << Res::QUADBATCH_FRAG 
         << "'...";
    cout.flush();
    prog = GLProgram_fromFiles(Res::QUADBATCH_VERT, Res::QUADBATCH_FRAG);
    hope(prog);
    cout << " done." << endl;

    u_fogdistance    = glGetUniformLocation(prog, "u_fogdistance");
    u_fogcolor       = glGetUniformLocation(prog, "u_fogcolor");
    u_viewprojmatrix = glGetUniformLocation(prog, "u_viewprojmatrix");
    u_viewposition   = glGetUniformLocation(prog, "u_viewposition");
    assert(u_fogdistance    != -1);
    assert(u_fogcolor       != -1);
    assert(u_viewprojmatrix != -1);
    assert(u_viewposition   != -1);
    for(uint_fast32_t i=0 ; i<MAX_TEXTURES ; ++i) {
        std::stringstream ss;
        ss << "u_textures[" << i << "]";
        u_textures[i] = glGetUniformLocation(prog, ss.str().c_str());
        assert(u_textures[i] != -1);
    }
}

void GLQuadBatch::cleanupGL() {
    glDeleteProgram(prog);
}



GLQuadBatch::GLQuadBatch() {
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    hope(vbo);
    hope(vao);
#if 0

    glBindVertexArray(vao);
    {
        glEnableVertexAttribArray(ATTRIB_POSITION     );
        glEnableVertexAttribArray(ATTRIB_TEXCOORDS    );
        glEnableVertexAttribArray(ATTRIB_TEXINDEX     );
        glEnableVertexAttribArray(ATTRIB_MODELMATRIX  );
        glEnableVertexAttribArray(ATTRIB_IS_HUD       );
        glEnableVertexAttribArray(ATTRIB_SHADOW_FACTOR);
        glEnableVertexAttribArray(ATTRIB_FOG_FACTOR   );
        // Make everything instanced
        glVertexAttribDivisor(ATTRIB_POSITION     , 1);
        glVertexAttribDivisor(ATTRIB_TEXCOORDS    , 1);
        glVertexAttribDivisor(ATTRIB_TEXINDEX     , 1);
        glVertexAttribDivisor(ATTRIB_MODELMATRIX  , 1);
        glVertexAttribDivisor(ATTRIB_IS_HUD       , 1);
        glVertexAttribDivisor(ATTRIB_SHADOW_FACTOR, 1);
        glVertexAttribDivisor(ATTRIB_FOG_FACTOR   , 1);

        glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo);

        glVertexAttribPointer (ATTRIB_POSITION     , 2, GL_FLOAT, GL_FALSE, sizeof(GLQuadInstance), OFFSET_POSITION     );
        glVertexAttribPointer (ATTRIB_TEXCOORDS    , 2, GL_FLOAT, GL_FALSE, sizeof(GLQuadInstance), OFFSET_TEXCOORDS    );

        glBindBuffer(GL_ARRAY_BUFFER, instances_vbo);

        glVertexAttribIPointer(ATTRIB_TEXINDEX     , 1, GL_UNSIGNED_INT,    sizeof(GLQuadInstance), OFFSET_TEXINDEX     );
        // This works only because GLM's matrices are column-major !
        glVertexAttribPointer (ATTRIB_MODELMATRIX+0, 4, GL_FLOAT, GL_FALSE, sizeof(GLQuadInstance), OFFSET_MODELMATRIX + 0*sizeof(vec4));
        glVertexAttribPointer (ATTRIB_MODELMATRIX+1, 4, GL_FLOAT, GL_FALSE, sizeof(GLQuadInstance), OFFSET_MODELMATRIX + 1*sizeof(vec4));
        glVertexAttribPointer (ATTRIB_MODELMATRIX+2, 4, GL_FLOAT, GL_FALSE, sizeof(GLQuadInstance), OFFSET_MODELMATRIX + 2*sizeof(vec4));
        glVertexAttribPointer (ATTRIB_MODELMATRIX+3, 4, GL_FLOAT, GL_FALSE, sizeof(GLQuadInstance), OFFSET_MODELMATRIX + 3*sizeof(vec4));
        glVertexAttribIPointer(ATTRIB_IS_HUD       , 1, GL_UNSIGNED_INT,    sizeof(GLQuadInstance), OFFSET_IS_HUD       );
        glVertexAttribPointer (ATTRIB_SHADOW_FACTOR, 1, GL_FLOAT, GL_FALSE, sizeof(GLQuadInstance), OFFSET_SHADOW_FACTOR);
        glVertexAttribPointer (ATTRIB_FOG_FACTOR   , 1, GL_FLOAT, GL_FALSE, sizeof(GLQuadInstance), OFFSET_FOG_FACTOR   );
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}

GLQuadBatch::~GLQuadBatch() {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}


void GLQuadBatch::updateInstancesVBO() const {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 
        instances.size()*sizeof(instances[0]), 
        &instances[0], 
        GL_DYNAMIC_DRAW // Assume it'll be updated often
    );
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLQuadBatch::renderGL() const {
    glUseProgram(prog);
    glBindVertexArray(vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, instances.size());
    glBindVertexArray(0);
    glUseProgram(0);
}

} // namespace dm

