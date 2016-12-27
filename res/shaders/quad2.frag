#version 330

// Must match QuadProgram in src/dm/VisualState.hpp.
#define MAX_TEXTURES 32

uniform sampler2D u_textures[MAX_TEXTURES];
uniform float u_fogdistance; // Distance at which an object is fully covered by fog
uniform vec3  u_fogcolor;

flat in uint v_texindex;
in vec2  v_texcoords;
in vec3  v_normal_viewprojspace;
in float v_distance_viewspace;
in float v_shadow_factor;
in float v_fog_factor;

out vec4 f_color;

void main() {
    // Dynamic uniform indexing (aka u_textures[i]) is not allowed in our GLSL version.
    // So here's your daily ugly, unscalable hack.
    switch(int(v_texindex)) {
        default: f_color = vec4(.5, .2, .8, 1);                  break;
        case  0: f_color = texture(u_textures[ 0], v_texcoords); break;
        case  1: f_color = texture(u_textures[ 1], v_texcoords); break;
        case  2: f_color = texture(u_textures[ 2], v_texcoords); break;
        case  3: f_color = texture(u_textures[ 3], v_texcoords); break;
        case  4: f_color = texture(u_textures[ 4], v_texcoords); break;
        case  5: f_color = texture(u_textures[ 5], v_texcoords); break;
        case  6: f_color = texture(u_textures[ 6], v_texcoords); break;
        case  7: f_color = texture(u_textures[ 7], v_texcoords); break;
        case  8: f_color = texture(u_textures[ 8], v_texcoords); break;
        case  9: f_color = texture(u_textures[ 9], v_texcoords); break;
        case 10: f_color = texture(u_textures[10], v_texcoords); break;
        case 11: f_color = texture(u_textures[11], v_texcoords); break;
        case 12: f_color = texture(u_textures[12], v_texcoords); break;
        case 13: f_color = texture(u_textures[13], v_texcoords); break;
        case 14: f_color = texture(u_textures[14], v_texcoords); break;
        case 15: f_color = texture(u_textures[15], v_texcoords); break;
        case 16: f_color = texture(u_textures[16], v_texcoords); break;
        case 17: f_color = texture(u_textures[17], v_texcoords); break;
        case 18: f_color = texture(u_textures[18], v_texcoords); break;
        case 19: f_color = texture(u_textures[19], v_texcoords); break;
        case 20: f_color = texture(u_textures[20], v_texcoords); break;
        case 21: f_color = texture(u_textures[21], v_texcoords); break;
        case 22: f_color = texture(u_textures[22], v_texcoords); break;
        case 23: f_color = texture(u_textures[23], v_texcoords); break;
        case 24: f_color = texture(u_textures[24], v_texcoords); break;
        case 25: f_color = texture(u_textures[25], v_texcoords); break;
        case 26: f_color = texture(u_textures[26], v_texcoords); break;
        case 27: f_color = texture(u_textures[27], v_texcoords); break;
        case 28: f_color = texture(u_textures[28], v_texcoords); break;
        case 29: f_color = texture(u_textures[29], v_texcoords); break;
        case 30: f_color = texture(u_textures[30], v_texcoords); break;
        case 31: f_color = texture(u_textures[31], v_texcoords); break;
    }
    vec3 shadowed = f_color.rgb * abs(dot(normalize(v_normal_viewprojspace), vec3(0,0,1)));
    f_color.rgb = mix(f_color.rgb, shadowed, v_shadow_factor);
    vec3 fogged = mix(f_color.rgb, u_fogcolor, v_distance_viewspace/u_fogdistance);
    f_color.rgb = mix(f_color.rgb, fogged, v_fog_factor);
}
