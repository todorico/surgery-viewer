#version 130

#define highp
#define mediump
#define lowp

attribute highp vec3 aPos;
attribute highp vec3 aNormal;
attribute highp vec2 aTexCoord;

varying highp vec2 TexCoord;

// uniform mat4 model;
// uniform mat4 view;
uniform highp mat4 MVP;

void main()
{
    TexCoord = aTexCoord;    
    gl_Position = MVP * vec4(aPos, 1.0);
}

