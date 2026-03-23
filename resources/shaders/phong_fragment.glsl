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
// In the case of structured output (like `Vertex_Out`), the same structure name must be used.
layout ( location = 0 ) in Vertex_Out {
	vec2 texture_uv;  // Fragment interpolated texture UV (no transformations needed)
} fragment_in;

layout ( location = 0 ) out vec4 fragment_color;

// Inputs from G-Buffer textures
uniform sampler2D gbuffer_position;
uniform sampler2D gbuffer_normal;
uniform sampler2D gbuffer_diffuse_specular;

uniform vec3 view_position;
uniform vec3 ambient; // ambient light color

// TODO: Make 'Material' Uniform Buffer
uniform float shininess_exponent;

struct Light {
    vec4 position; // .w: positional -> 1, directional -> 0
    vec4 color; // .a: intensity
};

#define MAX_LIGHT_SOURCES 32

layout ( std140, binding = 0 ) uniform Uniform_Buffer_Lights {
	Light lights[ MAX_LIGHT_SOURCES ];
	uint lights_count;
} ubo_lights;

void main()
{
	// Sample fragment data from G-Buffer textures.
	vec3 position  = texture( gbuffer_position,         fragment_in.texture_uv ).rgb; // 3D XYZ point
	vec3 normal    = texture( gbuffer_normal,           fragment_in.texture_uv ).rgb; // 3D XYZ vector direction
	vec3 diffuse   = texture( gbuffer_diffuse_specular, fragment_in.texture_uv ).rgb; // RGB color
	float specular = texture( gbuffer_diffuse_specular, fragment_in.texture_uv ).a;   // Specular highlight intensity

	vec3 view_direction = normalize( view_position - position );

	/* Ambient light */

	vec3 ambient_light = diffuse * ambient;
	vec3 final_color = ambient_light;

	for ( int i = 0; i < ubo_lights.lights_count; i += 1 ) {
		vec3 light_position = ubo_lights.lights[ i ].position.xyz;
		float w = ubo_lights.lights[ i ].position.w;
		vec3 light_color = ubo_lights.lights[ i ].color.rgb;
		float light_intensity = ubo_lights.lights[ i ].color.a;

		// TODO: Replace if-branching with Vector4 multiplication
		vec3 light_direction = normalize( light_position - position );
		float distance_to_light_source = length( light_position - /* fragment */ position );
		if ( w != 1.0 ) {
			light_direction = normalize( light_position );
			distance_to_light_source = 1.0;
		}

		// `1 / ( distance^2 )` is a close approximation of how a physical light behaves in a real world.
		// If the light's brightness is seen to be too intense, then the lighting calculations are probably done
		// in non-linear sRGB color space - with the 2.2 gamma pre-multiplication - instead of linear space.
		// It can be fixed using linear interpolation function instead: `1 / ( distance )`.
		// It makes things work, but should not be really used, as working in an sRGB space is a fundamental mistake.
		//
		// In the case of:
		// Positional  light:  `= 1.0 / ( distance^2 )`
		// Directional light:  `= 1.0 / ( 1.0^2 ) = 1.0 / 1.0 = 1.0`
		float attenuation = 1.0 / ( distance_to_light_source * distance_to_light_source );  // Quadratic, physically accurate
		// float attenuation = 1.0 / ( distance_to_light_source );  // Linear, kinda works for incorrect calculations in sRGB

		/* Diffuse light */

		// `dot( normal, light_direction )` calculates how similar the vector directions are.
		// The result takes "scaling" into account, for example:
		//   `dot( vec2( 3, 0 ), vec2( 4, 0 ) ) == 12`.
		// If the vectors are normalized, then the dot product is normalized as well in range [-1; 1], where:
		//  1  ->  Vectors are pointing in the same exact direction.
		//  0  ->  Vectors are pointing in perpendicular directions of each other.
		// -1  ->  Vectors are pointing in opposite directions of each other.
		// Some examples:
		// `dot( vec2( 1, 0 ), vec2(  1, 0 ) ) == 1`.
		// `dot( vec2( 1, 0 ), vec2(  0, 1 ) ) == 0`.
		// `dot( vec2( 1, 0 ), vec2( -1, 0 ) ) == -1`.
		//
		// In the case of comparing light direction and surface normal direction:
		//  1 (  0 degrees)  ->  Light hits the surface _directly_, full diffuse effect (100%).
		//  0 ( 90 degrees)  ->  Light is _parallel_ to the surface, no diffuse effect (0%).
		// -1 (180 degrees)  ->  Light comes from _behind_ the surface, inverted diffuse effect (-100%).
		//
		// Light coming from behind should not impact visible surface, that's why we clamp negatives to 0.
		float diffuse_term = max( dot( normal, light_direction ), 0.0 );

		vec3 diffuse_light = diffuse_term * diffuse * light_color;
		final_color += diffuse_light * light_intensity * attenuation;

		/* Specular highlight */

		// Phong - slower, sharper highlight
		// if ( shininess_exponent > 0.0 ) {
		// 	vec3 reflection_direction = reflect( -light_direction, normal );
		// 	float specular_term = pow( max( dot( view_direction, reflection_direction ), 0.0 ), shininess_exponent );
		//
		// 	vec3 specular_highlight = specular * specular_term * light_color;
		// 	final_color += specular_highlight * light_intensity;
		// }

		// Blinn-Phong - faster, softer highlight
		// Ensure `shininess_exponent > 0.0` on CPU-side so there is no need for if-branching on GPU.
		// if ( shininess_exponent > 0.0 ) {
			vec3 halfway_direction = normalize( light_direction + view_direction );
			float specular_term = pow( max( dot( normal, halfway_direction ), 0.0 ), shininess_exponent );

			vec3 specular_highlight = specular * specular_term * light_color;
			final_color += specular_highlight * light_intensity * attenuation;
		// }
	}

	// TODO: Do it in a separate post-processing pass.
	const float gamma = 2.2;
	final_color = pow( final_color, vec3( 1.0 / gamma ) );
	fragment_color = vec4( final_color.rgb, 1.0 );
}