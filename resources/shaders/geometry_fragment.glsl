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

// Inputs from the vertex shader.
// Variable names must match the ones declared in vertex shader outputs.
in vec3 fragment_position;    // Fragment interpolated World-space position
in vec3 fragment_normal;      // Fragment interpolated World-space normal
in vec2 fragment_texture_uv;  // Fragment interpolated texture UV (no transformations needed)

layout ( location = 0 ) out vec3 gbuffer_position;        // G-Buffer Position texture attachment
layout ( location = 1 ) out vec3 gbuffer_normal;          // G-Buffer Normal texture attachment
layout ( location = 2 ) out vec4 gbuffer_color_specular;  // G-Buffer Color/Specular texture attachment

// Material textures:
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_normal0;
uniform sampler2D texture_specular0;

void main()
{
	// Store Fragment XYZ World-space position as RGB color in G-Buffer Position texture.
	gbuffer_position = fragment_position;
	// gbuffer_position = vec3( 0.0, 1.0, 0.0 );

	// Store Fragment XYZ World-space normal vector as RGB color in G-Buffer Normal texture.
    gbuffer_normal = normalize( fragment_normal );
    // gbuffer_normal = vec3( 0.0, 0.0, 1.0 );

    // Store Fragment RGB diffuse texture color as RGB color in G-Buffer Color/Specular texture.
    gbuffer_color_specular.rgb = texture( texture_diffuse0, fragment_texture_uv ).rgb;
    //gbuffer_color_specular.rgb = vec3( 1.0, 0.0, 0.0 );

    // Store Fragment Specular intensity as Alpha channel value in G-Buffer Color/Specular texture.
    gbuffer_color_specular.a = texture( texture_specular0, fragment_texture_uv ).r;
    //gbuffer_color_specular.a = 0.5;
}