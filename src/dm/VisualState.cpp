#include <dm/dm.hpp>

using namespace std;
using namespace glm;

namespace dm {

const GLuint  QuadVertex  ::ATTRIB_POSITION   = 0;
const GLuint  QuadVertex  ::ATTRIB_TEXCOORDS  = 1;
const GLuint  QuadInstance::ATTRIB_TILECOORDS = 2;
const GLuint  QuadInstance::ATTRIB_NORMAL     = 3;
const GLuint  QuadInstance::ATTRIB_TEXINDEX   = 4;
const GLvoid* QuadVertex  ::OFFSET_POSITION   =(void*)offsetof(QuadVertex, position);
const GLvoid* QuadVertex  ::OFFSET_TEXCOORDS  =(void*)offsetof(QuadVertex, texcoords);
const GLvoid* QuadInstance::OFFSET_TILECOORDS =(void*)offsetof(QuadInstance, tilecoords);
const GLvoid* QuadInstance::OFFSET_NORMAL     =(void*)offsetof(QuadInstance, normal);
const GLvoid* QuadInstance::OFFSET_TEXINDEX   =(void*)offsetof(QuadInstance, texindex);


QuadBatch::QuadBatch() {
    glGenBuffers(1, &instances_vbo);
    glGenBuffers(1, &data_vbo);
    glGenVertexArrays(1, &vao);

    glBindBuffer(GL_ARRAY_BUFFER, data_vbo);
    static const QuadVertex data[4] = {
        QuadVertex(vec2( .5f, -.5f), vec2(1.f, 0.f)),
        QuadVertex(vec2( .5f,  .5f), vec2(1.f, 1.f)),
        QuadVertex(vec2(-.5f, -.5f), vec2(0.f, 0.f)),
        QuadVertex(vec2(-.5f,  .5f), vec2(0.f, 1.f))
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(vao);
    {
        glEnableVertexAttribArray(QuadVertex::ATTRIB_POSITION);
        glEnableVertexAttribArray(QuadVertex::ATTRIB_TEXCOORDS);
        glEnableVertexAttribArray(QuadInstance::ATTRIB_TILECOORDS);
        glEnableVertexAttribArray(QuadInstance::ATTRIB_NORMAL);
        glEnableVertexAttribArray(QuadInstance::ATTRIB_TEXINDEX);
        glVertexAttribDivisor(QuadVertex::ATTRIB_POSITION,     0);
        glVertexAttribDivisor(QuadVertex::ATTRIB_TEXCOORDS,    0);
        glVertexAttribDivisor(QuadInstance::ATTRIB_TILECOORDS, 1);
        glVertexAttribDivisor(QuadInstance::ATTRIB_NORMAL,     1);
        glVertexAttribDivisor(QuadInstance::ATTRIB_TEXINDEX,   1);
        glBindBuffer(GL_ARRAY_BUFFER, data_vbo);
        glVertexAttribPointer(QuadVertex::ATTRIB_POSITION,  2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), QuadVertex::OFFSET_POSITION);
        glVertexAttribPointer(QuadVertex::ATTRIB_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), QuadVertex::OFFSET_TEXCOORDS);
        glBindBuffer(GL_ARRAY_BUFFER, instances_vbo);
        glVertexAttribIPointer(QuadInstance::ATTRIB_TILECOORDS, 3, GL_INT, sizeof(QuadInstance), QuadInstance::OFFSET_TILECOORDS);
        glVertexAttribIPointer(QuadInstance::ATTRIB_NORMAL,     3, GL_INT, sizeof(QuadInstance), QuadInstance::OFFSET_NORMAL);
        glVertexAttribIPointer(QuadInstance::ATTRIB_TEXINDEX,   1, GL_INT, sizeof(QuadInstance), QuadInstance::OFFSET_TEXINDEX);
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}
QuadBatch::~QuadBatch() {
    glDeleteBuffers(1, &instances_vbo);
    glDeleteBuffers(1, &data_vbo);
    glDeleteVertexArrays(1, &vao);
}
void QuadBatch::updateInstancesVBO() const {
    glBindBuffer(GL_ARRAY_BUFFER, instances_vbo);
    glBufferData(GL_ARRAY_BUFFER, instances.size()*sizeof(instances[0]), &instances[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void QuadBatch::renderGL() const {
    glBindVertexArray(vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, instances.size());
    glBindVertexArray(0);
}



QuadProgram::QuadProgram() 
  : prog(loadGLProgram(
        "res/shaders/quad.vert",
        "res/shaders/quad.frag"
    ))
{
    u_viewprojmatrix = glGetUniformLocation(prog.prog_id, "u_viewprojmatrix");
    //assert(u_viewprojmatrix != -1);
    for(unsigned i=0 ; i<QUADPROGRAM_MAX_TEXTURES ; ++i) {
        std::stringstream ss;
        ss << "u_textures[" << i << "]";
        u_textures[i] = glGetUniformLocation(prog.prog_id, ss.str().c_str());
        assert(u_textures[i] != -1);
    }
}
QuadProgram::~QuadProgram() {}




Camera::Camera(ivec2 viewport_size) 
    : position(0,0,0), theta(M_PI/2.f), viewport_size(viewport_size) {}
Camera::~Camera() {}

void Camera::reshape(ivec2 new_viewport_size) {
    viewport_size = new_viewport_size;
}
vec3 Camera::getFrontVector() const {
    return vec3(cosf(theta), 0, -sinf(theta));
}
mat4 Camera::getViewMatrix() const {
    return lookAt(position, position+getFrontVector(), vec3(0,1,0));
}
mat4 Camera::getProjectionMatrix() const {
    return perspective(
        radians(60.f), viewport_size.x/(float)viewport_size.y, .01f, 100.f
    );
}
mat4 Camera::getViewProjectionMatrix() const {
    return getProjectionMatrix()*getViewMatrix();
}




VisualState::VisualState(ivec2 viewport_size)
  : frame_count(0), frames_per_second(60),
    camera(viewport_size), textures({
        0xff0000ff,
        0xffff00ff,
        0x00ff00ff,
        0x00ffffff,
        0x0000ffff,
        0xff00ffff,
        0xccccccff,
        0x888888ff
    })
{
    quad_program.prog.use();
    quad_batch.instances = vector<QuadInstance>({
        QuadInstance(ivec3( 0, 0,-1), ivec3(-1, 0, 0), 0),
        QuadInstance(ivec3( 0, 0,-2), ivec3(-1, 0, 0), 1),
        QuadInstance(ivec3( 0, 0,-1), ivec3( 0,-1, 0), 2),
        QuadInstance(ivec3( 0, 0,-2), ivec3( 0,-1, 0), 3),
        QuadInstance(ivec3( 0, 0,-1), ivec3( 1, 0, 0), 4),
        QuadInstance(ivec3( 0, 0,-2), ivec3( 1, 0, 0), 5),
        QuadInstance(ivec3( 0, 0,-2), ivec3( 0, 0,-1), 6),
        QuadInstance(ivec3( 0, 1,-2), ivec3( 0,-1, 0), 7),
        QuadInstance(ivec3( 0, 1,-1), ivec3( 0,-1, 0), 6),

        QuadInstance(ivec3( 1, 0, 0), ivec3( 0, 0, 1), 6),
        QuadInstance(ivec3( 1, 0, 0), ivec3( 0, 0,-1), 4),
        QuadInstance(ivec3( 1, 0, 0), ivec3( 0, 1, 0), 5),
        QuadInstance(ivec3( 1, 0, 0), ivec3( 0,-1, 0), 3),
        QuadInstance(ivec3( 1, 0, 0), ivec3( 1, 0, 0), 2),

        QuadInstance(ivec3(-1, 0, 0), ivec3( 0, 0, 1), 6),
        QuadInstance(ivec3(-1, 0, 0), ivec3( 0, 0,-1), 4),
        QuadInstance(ivec3(-1, 0, 0), ivec3( 0, 1, 0), 5),
        QuadInstance(ivec3(-1, 0, 0), ivec3( 0,-1, 0), 3),
        QuadInstance(ivec3(-1, 0, 0), ivec3(-1, 0, 0), 2),

        QuadInstance(ivec3( 0, 0, 1), ivec3( 1, 0, 0), 6),
        QuadInstance(ivec3( 0, 0, 1), ivec3(-1, 0, 0), 4),
        QuadInstance(ivec3( 0, 0, 1), ivec3( 0, 1, 0), 5),
        QuadInstance(ivec3( 0, 0, 1), ivec3( 0,-1, 0), 3),
        QuadInstance(ivec3( 0, 0, 1), ivec3( 0, 0, 1), 2)
    });
    quad_batch.updateInstancesVBO();
    for(size_t i=0 ; i<QUADPROGRAM_MAX_TEXTURES ; ++i) {
        size_t texunit = i+1;
        glActiveTexture(GL_TEXTURE0 + texunit);
        glBindTexture(GL_TEXTURE_2D, textures[i].texid);
        glUniform1i(quad_program.u_textures[i], texunit);
    }
    glActiveTexture(GL_TEXTURE0);
}
VisualState::~VisualState() {}

void VisualState::reshape(ivec2 new_viewport_size) {
    camera.reshape(new_viewport_size);
}

void VisualState::update(const GameState &game_state) {
    ++frame_count;
    //cout << "Frame: " << frame_count << endl;
    uint32_t time_ms = SDL_GetTicks();
    uint32_t len = game_state.time_ms - game_state.prev_time_ms;
    uint32_t cur = time_ms            - game_state.prev_time_ms;
    float interp = cur/(float)len - 1.f;
    //cout << "Inter: " << interp << endl;
    const Character &h = game_state.hero;
    float trans_cur = h.moving_transition/100.f
        + interp*Character::MOVING_TRANSITION_STEP/100.f;
    camera.position.x =  h.position.x + h.moving_direction.x*trans_cur;
    camera.position.z = -h.position.y - h.moving_direction.y*trans_cur;
    //camera.position.z -= 0.001f;
    float pa = game_state.hero.prev_angle_deg, a=game_state.hero.angle_deg;
    if(fabs(a-360-pa) < fabs(a-pa))
        a -= 360;
    if(fabs(a+360-pa) < fabs(a-pa))
        a += 360;
    float theta_interp = pa + interp*(a-pa);
    camera.theta = radians(theta_interp);
    quad_program.prog.use();
    glUniformMatrix4fv(
        quad_program.u_viewprojmatrix, 1, GL_FALSE, 
        value_ptr(camera.getViewProjectionMatrix())
    );
}

void VisualState::renderGL() const {
    //cout << "Prog: " << quad_program.prog.prog_id << endl;
    quad_program.prog.use();
    quad_batch.renderGL();
}

}
