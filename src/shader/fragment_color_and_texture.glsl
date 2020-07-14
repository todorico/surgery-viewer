#version 100

//#define highp
//#define mediump
//#define lowp

// out vec4 FragColor;

// fragment variables

varying highp vec4 f_color;
varying highp vec2 f_texcoord;

// global variables

uniform highp sampler2D f_texture;

void main()
{    
    // gl_FragColor = mix(texture2D(f_texture, f_texcoord), f_color, vec4(0.5, 0.5, 0.5, 0.5));
    // gl_FragColor = f_color;
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
