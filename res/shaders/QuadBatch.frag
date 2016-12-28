#version 330

uniform sampler2D u_texture;
uniform float u_shadow_factor;
uniform float u_fog_factor;
uniform float u_fogdistance; // Distance at which an object is fully covered by fog
uniform vec3  u_fogcolor;
uniform vec4  u_rgba_fx;
uniform float u_rgba_fx_factor;

in vec2  v_texcoords;
in vec3  v_normal_viewprojspace;
in float v_distance_viewspace;

out vec4 f_color;

void main() {
    f_color = texture2D(u_texture, v_texcoords);
    f_color = mix(f_color, u_rgba_fx, u_rgba_fx_factor);
    
    vec3 shadowed = f_color.rgb * abs(dot(normalize(v_normal_viewprojspace), vec3(0,0,1)));
    f_color.rgb = mix(f_color.rgb, shadowed, u_shadow_factor);

    vec3 fogged = mix(f_color.rgb, u_fogcolor, v_distance_viewspace/u_fogdistance);
    f_color.rgb = mix(f_color.rgb, fogged, u_fog_factor);
}
