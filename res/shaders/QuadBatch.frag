#version 330

uniform vec3 u_viewposition;
uniform sampler2D u_texture;
uniform float u_shadow_factor;
uniform float u_fog_factor;
uniform float u_fogdistance; // Distance at which an object is fully covered by fog
uniform vec3  u_fogcolor;
uniform vec4  u_rgba_fx;
uniform float u_rgb_fx_factor;
uniform float u_alpha_fx_factor;
uniform float u_src_alpha_factor;

in vec2 v_texcoords;
in vec3 v_normal_viewspace;
in vec3 v_position;

out vec4 f_color;

void main() {
    f_color = texture2D(u_texture, v_texcoords);
    f_color.a  *= u_src_alpha_factor;
    f_color.rgb = mix(f_color.rgb, u_rgba_fx.rgb, u_rgb_fx_factor);
    f_color.a   = mix(f_color.a,   u_rgba_fx.a,   u_alpha_fx_factor);
    
    vec3 shadowed = f_color.rgb * abs(dot(normalize(v_normal_viewspace), vec3(0,0,1)));
    f_color.rgb = mix(f_color.rgb, shadowed, u_shadow_factor);

    vec3 fogged = mix(f_color.rgb, u_fogcolor, clamp(length(v_position-u_viewposition)/u_fogdistance, 0, 1));
    f_color.rgb = mix(f_color.rgb, fogged, u_fog_factor);
}
