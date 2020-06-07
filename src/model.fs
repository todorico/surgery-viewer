#version 130

#define highp
#define mediump
#define lowp

// out vec4 FragColor;

varying highp vec2 TexCoord;

uniform highp sampler2D Texture;

void main()
{    
     gl_FragColor = texture(Texture, TexCoord);
     // gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
