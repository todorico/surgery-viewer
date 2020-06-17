#version 130

#define highp
#define mediump
#define lowp

// out vec4 FragColor;

// fragment variables

varying highp vec4 f_color;
varying highp vec2 f_texcoord;

// global variables

uniform highp sampler2D f_texture;

void main()
{    
     gl_FragColor = texture(f_texture, f_texcoord);
     // gl_FragColor = f_color;
}
