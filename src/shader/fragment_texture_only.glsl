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
    precision mediump sampler2D;
    precision mediump int;
#endif

#if __VERSION__ >= 130 // Version compatibility
    #define varying in
    out vec4 mgl_FragColor;
    #define texture2D texture
#else
    #define mgl_FragColor gl_FragColor  
#endif

// [SHADER CODE]

// fragment variables

varying vec4 f_color;
varying vec2 f_texcoord;

// global variables

uniform sampler2D f_texture;

void main()
{    
    mgl_FragColor = texture2D(f_texture, f_texcoord);
    // mgl_FragColor = f_color;
    // mgl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
