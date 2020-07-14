//#version 3.0

//#define highp
//#define mediump
//#define lowp

// vertex attributes

attribute highp vec3 v_position;
attribute highp vec3 v_normal;
attribute highp vec4 v_color;
attribute highp vec2 v_texcoord;

// fragment attributes

varying highp vec4 f_color;
varying highp vec2 f_texcoord;

// uniform mat4 model;
// uniform mat4 view;
uniform highp mat4 MVP;

void main()
{
    f_color = v_color;    
    f_texcoord = v_texcoord;    
    gl_Position = MVP * vec4(v_position, 1.0);
    // gl_PointSize = 10;
}

