#version 140

// [COMPATIBILITY CODE] /////////////////////////

////// [GLSL VERSIONS COMPATIBILITY]

#if __VERSION__ >= 130
    #define attribute in
    #define varying out
#endif

////// [GLSL ES COMPATIBILITY]

#ifdef GL_ES 
    // Default precision qualifiers
    precision mediump float;
    precision mediump int;

    // Explicit precision qualifiers
    #define HIGHP highp
    #define MEDIUMP mediump
    #define LOWP  lowp
#else
    #define HIGHP
    #define MEDIUMP
    #define LOWP
#endif

// [SHADER CODE] ////////////////////////////////

// Built-in_Variable_(GLSL)

// input

// in int gl_VertexID;
// in int gl_InstanceID;
// in int gl_DrawID; // Requires GLSL 4.60 or ARB_shader_draw_parameters
// in int gl_BaseVertex; // Requires GLSL 4.60 or ARB_shader_draw_parameters
// in int gl_BaseInstance; // Requires GLSL 4.60 or ARB_shader_draw_parameters

// output

// gl_Position
// gl_PointSize
// gl_ClipDistance // Requires GLSL 4.10 or ARB_separate_shader_objects

////// [INPUT]

// Vertex attributes
attribute vec3 v_position;
attribute vec3 v_normal;
attribute vec4 v_color;
attribute vec2 v_texcoord;

// Global variables
uniform mat4 MVP_matrix;
uniform mat4 V_matrix; // = MV_matrix because M is identity

uniform vec3 camera_position;
uniform vec3 camera_direction;

////// [OUTPUT]

// Fragment variables
varying vec4 f_color;
varying vec2 f_texcoord;

// Explicit space variables
varying vec3 camera_direction_cameraspace;
varying vec3 vertex_normal_cameraspace;
varying vec3 light_direction_cameraspace;


void main()
{
    // gl_PointSize = 10;
    gl_Position = MVP_matrix * vec4(v_position, 1.0);

    f_color    = v_color;    
    f_texcoord = v_texcoord;    

    // vertex_normal_cameraspace    = normalize((V_matrix * vec4(v_normal, 0.0)).xyz);
    // camera_direction_cameraspace = normalize((V_matrix * vec4(camera_direction, 1.0)).xyz);

    // vec3 light_position_worldspace    = camera_position + vec3(0.0, 1.0, 0.0);
    // vec3 light_position_cameraspace   = (V_matrix * vec4(light_position_worldspace, 1.0)).xyz;
    // vec3 vertex_position_cameraspace  = (V_matrix * vec4(v_position, 0.0)).xyz;
    
    // light_direction_cameraspace  = normalize(vertex_position_cameraspace - light_position_cameraspace);


    vertex_normal_cameraspace    = normalize(v_normal);
    camera_direction_cameraspace = normalize(camera_direction);

    vec3 light_position_cameraspace   = camera_position + vec3(0.0, 1.0, 0.0);
    vec3 vertex_position_cameraspace  = v_position;
    
    light_direction_cameraspace  = normalize(vertex_position_cameraspace - light_position_cameraspace);
}

