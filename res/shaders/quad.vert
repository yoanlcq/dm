#version 330

uniform mat4 u_viewprojmatrix;

layout(location=0) in vec2  a_position;
layout(location=1) in vec2  a_texcoords;
layout(location=2) in ivec3 a_tilecoords;
layout(location=3) in ivec3 a_normal;
layout(location=4) in int   a_texindex;

flat out int v_texindex;
out vec2 v_texcoords;
//flat out ivec3 v_normal;
//flat out ivec3 v_tilecoords;

// http://www.neilmendoza.com/glsl-rotation-about-an-arbitrary-axis/
mat3 rotate3d(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat3(
        oc * axis.x * axis.x + c,
        oc * axis.x * axis.y - axis.z * s,
        oc * axis.z * axis.x + axis.y * s,  
        oc * axis.x * axis.y + axis.z * s,  
        oc * axis.y * axis.y + c,
        oc * axis.y * axis.z - axis.x * s,  
        oc * axis.z * axis.x - axis.y * s,  
        oc * axis.y * axis.z + axis.x * s,  
        oc * axis.z * axis.z + c
    );
}

#define M_PI 3.1415926535897932384626433832795

void main() {
    v_texindex  = a_texindex;
    v_texcoords = a_texcoords;
    //v_normal = a_normal;
    //v_tilecoords = a_tilecoords;
    vec3 position = rotate3d(vec3(0,1,0), (M_PI/2.f)+atan(float(a_normal.z), float(a_normal.x)))
                  * rotate3d(vec3(1,0,0), asin(float(a_normal.y)))
                  * vec3(a_position, 0);
    position += a_tilecoords + vec3(a_normal)/2.f;
    gl_Position = u_viewprojmatrix*vec4(position, 1);
}
