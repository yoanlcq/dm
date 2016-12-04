#version 330

// Must match QuadProgram in src/dm/VisualState.hpp.
#define MAX_TEXTURES 8
uniform sampler2D u_textures[MAX_TEXTURES];

flat in int v_texindex;
in vec2  v_texcoords;
//flat in ivec3 v_normal;
//flat in ivec3 v_tilecoords;

out vec4 f_color;

void main() {
    // Writing u_textures[i] is not allowed in our GLSL version.
    switch(v_texindex) {
        default:f_color = vec4(.5,.2,.8,1); break;
        case 0: f_color = texture2D(u_textures[0], v_texcoords); break;
        case 1: f_color = texture2D(u_textures[1], v_texcoords); break;
        case 2: f_color = texture2D(u_textures[2], v_texcoords); break;
        case 3: f_color = texture2D(u_textures[3], v_texcoords); break;
        case 4: f_color = texture2D(u_textures[4], v_texcoords); break;
        case 5: f_color = texture2D(u_textures[5], v_texcoords); break;
        case 6: f_color = texture2D(u_textures[6], v_texcoords); break;
        case 7: f_color = texture2D(u_textures[7], v_texcoords); break;
    }
}
