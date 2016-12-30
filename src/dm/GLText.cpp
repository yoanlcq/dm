#include <dm/dm.hpp>

using namespace glm;
using namespace std;

namespace dm {


size_t GLText::refcount(0);
GLuint GLText::tex(0);


void GLText::setupGL() {
    hope(tex = GLTexture_fromFile("res/monospace.png"));
    GLTexture_bindToUnit(tex, TextureUnit::MONOSPACE_FONT_ATLAS);
}
void GLText::cleanupGL() {
    glDeleteTextures(1, &tex);
}


GLText:: GLText() 
    : position(vec3(0,0,0)),
      height(1),
      rgba(0,0,0,1)
{
    if(!refcount)
        setupGL();
    ++refcount;

    quad_batch.texture_unit = TextureUnit::MONOSPACE_FONT_ATLAS;
}

GLText::~GLText() {
    --refcount;
    if(!refcount)
        cleanupGL();
}

static const ivec2 CHAR_GRID(15, 7);
static const vec2 CHAR_SZ = vec2(68/1024.f, 113/1024.f);
static const int FIRST_CHAR = '!';
static const int LAST_CHAR  = '~';
static const int CHAR_COUNT = LAST_CHAR - FIRST_CHAR + 1;
static const float CHAR_RATIO = CHAR_SZ.x/CHAR_SZ.y;


float GLText::getLineWidth(size_t i) const {
    return lines[i].length()*CHAR_RATIO*height;
}

void GLText::updateQuadBatch() {
    quad_batch.instances.clear();
    vec3 nposition = position;
    nposition.x += height*CHAR_RATIO/2.f;
    nposition.y += height/2.f;
    GLQuadBatch::QuadInstance quad = {
          translate(mat4(), nposition) 
        * scale(vec3(CHAR_RATIO*height,height,1)),
        vec2(0,0), vec2(0,0)
    };
    size_t lineno=0;
    for(string& line : lines) {
        for(size_t i=0 ; i<line.length() ; ++i) {
            assert(isprint(line[i]));
            // HACK
            if(line[i] != ' ') {
                int c = line[i] - FIRST_CHAR;
                assert(c < CHAR_COUNT);
                int cy = c / CHAR_GRID.x;
                int cx = c % CHAR_GRID.x;
                quad.sprite_pos  = vec2(
                    CHAR_SZ.x*cx, 
                    1 - CHAR_SZ.y*(cy+1) * (7/6.f)// <-- WTF ??
                );
                quad.sprite_size = CHAR_SZ;
                quad_batch.instances.push_back(quad);
            }
            quad.modelmatrix  = translate(quad.modelmatrix, vec3(1, 0, 0));
        }
        ++lineno;
        nposition.y -= lineno*height;
        quad.modelmatrix =
              translate(mat4(), nposition) 
            * scale(vec3(CHAR_RATIO*height,height,1));
    }
    quad_batch.rgba_fx         = rgba;
    quad_batch.rgb_fx_factor   = 1;
    quad_batch.alpha_fx_factor = 0;
    quad_batch.src_alpha_factor= rgba.a;
    quad_batch.updateInstancesVBO();
}

void GLText::renderGL_HUD(const OrthographicView &view) const {
    quad_batch.renderGL_HUD(view);
}

} // namespace dm
