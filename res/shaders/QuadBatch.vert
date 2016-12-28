#version 330

uniform mat4 u_viewprojmatrix;
uniform vec3 u_viewposition;

layout(location=0) in vec2  a_position;     // 2D Vertex position (quad corner)
layout(location=1) in vec2  a_texcoords;
layout(location=2) in mat4  a_modelmatrix;
// A mat4 consumes 4 locations, so The next one should be 6 (if any).
layout(location=6) in vec2  a_sprite_pos;
layout(location=7) in vec2  a_sprite_size;

out vec2  v_texcoords;
out vec3  v_normal_viewprojspace;
out float v_distance_viewspace;

void main() {
    v_texcoords = a_sprite_pos + a_texcoords*a_sprite_size;
    mat4 mvp = u_viewprojmatrix * a_modelmatrix;
    
    vec4 normal_vp = mvp * vec4(a_position,1,0);
    v_normal_viewprojspace = normal_vp.xyz;
    if(normal_vp.w > 0)
        v_normal_viewprojspace /= normal_vp.w;

    gl_Position = mvp * vec4(a_position,0,1);
    v_distance_viewspace = length(gl_Position.xyz - u_viewposition);
}
