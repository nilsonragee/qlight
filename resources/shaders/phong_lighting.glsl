#version 460 core
/*
    GLSL built-in fragment shader variables:

    in   vec4 gl_FragCoord;
    in   bool gl_FrontFacing;
    in   vec2 gl_PointCoord;

    in    int gl_SampleID;        // OpenGL 4.0+
    in   vec2 gl_SamplePosition;  // OpenGL 4.0+
    in    int gl_SampleMaskIn[];  // OpenGL 4.0+

    in  float gl_ClipDistance[];  // OpenGL 4.0+ (?), User-controllable
    in    int gl_PrimitiveID;     // OpenGL 4.0+ (?), User-controllable

    in    int gl_Layer;           // OpenGL 4.3+
    in    int gl_ViewportIndex;   // OpenGL 4.3+


    out float gl_FragDepth;
    out   int gl_SampleMask[];    // GLSL 4.00+ or ARB_sample_shading
*/

struct Material {
     vec3 ambient;
     vec3 diffuse;
     vec3 specular;
    float shininess;
};

struct Light {
    vec3 position; // World position

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Inputs from the vertex shader.
// Variable names must match the ones declared in vertex shader outputs.
in vec3 out_vertex_position;
in vec3 out_vertex_normal;
in vec2 out_vertex_texture_uv;

out vec4 out_fragment_color;

uniform     vec3 view_position;
uniform Material material;
uniform    Light light;

void main()
{
    // ambient light
    vec3 ambient_light = light.ambient * material.ambient;

    // diffuse light
    vec3 normal = normalize(out_vertex_normal);
    vec3 light_direction = normalize(light.position - out_vertex_position);

    float diffuse = max(dot(normal, light_direction), 0.0);
    vec3 diffuse_light = light.diffuse * (diffuse * material.diffuse);

    // specular highlight
    vec3 view_direction = normalize(view_position - out_vertex_position);
    vec3 reflection_direction = reflect(-light_direction, normal);

    float specular = pow(max(dot(view_direction, reflection_direction), 0.0), material.shininess);
    vec3 specular_highlight = light.specular * (specular * material.specular);

    // result
    vec3 result = ambient_light + diffuse_light + specular_highlight;
    out_fragment_color = vec4(result, 1.0);
}