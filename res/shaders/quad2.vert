#version 330

uniform mat4 u_viewprojmatrix;
uniform vec3 u_viewposition;

layout(location=0) in vec2  a_position;     // 2D Vertex position (quad corner)
layout(location=1) in vec2  a_texcoords;
layout(location=2) in uint  a_texindex;     // Index into the uniform array of sampler2Ds (see fragment shader).
layout(location=3) in mat4  a_modelmatrix; // For billboards, just make them face the camera manually.
// A mat4 consumes 4 locations, so we jump to 7.
layout(location=7) in uint  a_is_hud; // For HUD elements
layout(location=8) in float a_shadow_factor; // from 0 (self-illumination), to 1 otherwise.
layout(location=9) in float a_fog_factor;    // from 0 (unaffected by fog), to 1 otherwise.

flat out uint v_texindex;
out vec2  v_texcoords;
out vec3  v_normal_viewprojspace;
out float v_distance_viewspace;
out float v_shadow_factor;
out float v_fog_factor;

void main() {
    v_texindex      = a_texindex;
    v_texcoords     = a_texcoords;
    v_shadow_factor = a_shadow_factor;
    v_fog_factor    = a_fog_factor;

    vec4 normal_vp = u_viewprojmatrix * a_modelmatrix * vec4(a_position,1,0);
    v_normal_viewprojspace = normal_vp.xyz / normal_vp.w;

    gl_Position = a_modelmatrix * vec4(a_position,0,1);
    v_distance_viewspace = length(gl_Position.xyz - u_viewposition);
    
    if(!bool(a_is_hud))
        gl_Position = u_viewprojmatrix * gl_Position;
}
