// [COMPATIBILITY CODE]

// #ifdef GL_ES // Per variable precision GL(ES) compatibility
//     #define MEDIUMP mediump
//     #define HIGHP highp
//     #define LOWP  lowp
// #else
//     #define MEDIUMP
//     #define HIGHP
//     #define LOWP
// #endif

#ifdef GL_ES // Global precision GL(ES) compatibility
    precision mediump float;
    precision mediump int;
#endif

#if __VERSION__ >= 130 // Version compatibility
    #define attribute in
    #define varying out
#endif

// [SHADER CODE]

// vertex attributes

attribute vec3 v_position;
attribute vec3 v_normal;
attribute vec4 v_color;
attribute vec2 v_texcoord;

// fragment attributes

varying vec4 f_color;
varying vec2 f_texcoord;

// uniform mat4 model;
// uniform mat4 view;
uniform mat4 MVP;

void main()
{
    f_color = v_color;    
    f_texcoord = v_texcoord;    
    gl_Position = MVP * vec4(v_position, 1.0);
    // gl_PointSize = 10;
}

