// [COMPATIBILITY CODE]

////// [GLSL VERSIONS COMPATIBILITY]

#if __VERSION__ >= 130
    #define varying in
    #define texture2D texture

    // Compatible gl_FragColor
    out vec4 CGL_FRAG_COLOR;
#else
    #define CGL_FRAG_COLOR gl_FragColor
#endif

////// [GLSL ES COMPATIBILITY]

#ifdef GL_ES
    // Default precision qualifiers
    precision mediump float;
    precision mediump int;
    precision mediump sampler2D;

    // Explicit precision qualifiers
    #define HIGHP highp     
    #define MEDIUMP mediump
    #define LOWP  lowp
#else
    #define HIGHP
    #define MEDIUMP
    #define LOWP
#endif

// [SHADER CODE]

////// [FUNCTIONS]

float diffuse_value(vec3 object_light_direction, vec3 object_normal)
{
    return max(dot(object_light_direction, object_normal), 0.0);
}

float specular_value(vec3 light_objet_reflection, vec3 object_view_direction, float shininess)
{
    return pow(max(dot(light_objet_reflection, object_view_direction), 0.0), shininess);
}


// (assert line x failed)
// 
// __FILE__
// __LINE__
// 

// Built-in_Variable_(GLSL)

// Input
// in vec4 gl_FragCoord;
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;

// Since 4.0
// in int gl_SampleID;
// in vec2 gl_SamplePosition;
// in int gl_SampleMaskIn[];

// in float gl_ClipDistance[];
// in int gl_PrimitiveID;

// Since 4.3
// in int gl_Layer;
// in int gl_ViewportIndex;

// Output
// out float gl_FragDepth;
// 
// GLSL 4.00 or ARB_sample_shading
// out int gl_SampleMask[];

////// [INPUT]

// fragment attributes
varying vec4 f_color;
varying vec2 f_texcoord;

// Explicit space variables
varying vec3 camera_direction_cameraspace;
varying vec3 vertex_normal_cameraspace;
varying vec3 light_direction_cameraspace;

// Global variables
uniform sampler2D f_texture;

void main()
{
    // Phong

    vec3 light_color  = vec3(1.0, 1.0, 1.0);

    float ambient_strength  = 0.5;
    float diffuse_strength  = 0.2;
    float specular_strength = 0.3;

    vec3 ambient  = light_color * ambient_strength;
    vec3 diffuse  = light_color * diffuse_strength  * diffuse_value(-light_direction_cameraspace, vertex_normal_cameraspace);

    // Phong specular
    // vec3 specular = light_color * specular_strength * specular_value(reflect(light_direction_cameraspace, vertex_normal_cameraspace), -camera_direction_cameraspace, 1.0);
    
    // Blinn-Phong specular
    vec3 halfway_direction = normalize(-light_direction_cameraspace + -camera_direction_cameraspace);
    vec3 specular = light_color * specular_strength * specular_value(halfway_direction, vertex_normal_cameraspace, 1.0);

    CGL_FRAG_COLOR = texture2D(f_texture, f_texcoord) * vec4(ambient + diffuse + specular, 1.0);
}


