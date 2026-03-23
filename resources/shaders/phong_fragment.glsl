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

#define PI 3.141592653589793
// PI * 2.0
#define TWO_PI 6.283185307179586
// PI / 2.0
#define PI_OVER_TWO 1.5707963267948966

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

// sRGB to Linear
vec3 SRGBToLinear( vec3 srgb ) {
	// The inverse of linear -> non-linear sRGB conversion.
	// For detailed explanation, look at the other function.

	// `low` - formula used for close to black color values in the range of: [0.0, 0.0031308]
	// `x * 12.92`
    vec3 low = srgb / 12.92;
    // `high` - formula used for the rest of color values in the range of: (0.0031308, 1.0]
    // `1.055 * x ^ ( 1 / 2.4 ) - 0.055`
    vec3 high = pow( ( srgb + 0.055 ) / 1.055, vec3( 2.4 ) );
    // `cutoff` - a "switch" between two formulas:
    //   For `step( x, y )`:
    //   - If `x < y`, return 0.0
    //   - If `x >= y`, return 1.0
    vec3 cutoff = step( vec3( 0.04045 ), srgb );
    // Interpolate/"Mix" the calculated values using the `cutoff` weight/multiplier:
    //   For `mix( x, y, a )`, the computation formula is: `x * (1 - a)  +  y * a`, so:
    //   - If `cutoff = 0.0`, then `x * (1 - 0)  +  y * 0  =  1x + 0y  =  x`. The output is `low` result.
    //   - If `cutoff = 1.0`, then `x * (1 - 1)  +  y * 1  =  0x + 1y  =  y`. The output is `high` result.
    return mix( low, high, cutoff );
}

// Linear to sRGB
vec3 LinearToSRGB( vec3 linear ) {
	// `low` - formula used for close to black color values in the range of: [0.0, 0.0031308]
	// Interpolates linearly from 0.0031308 to 0.0 as the general sRGB gamma curve does not
	//   actually reach the 0.0 value, clamping dark values to 0.00083381 and effectively losing
	//   the color information in the darks.  Thus, this formula is set to be used at values 0.0031308
	//   and lower as it provides better precision and ensures the darks are preserved as much as possible.
	// `x * 12.92`
    vec3 low = linear * 12.92;
    // `high` - formula used for the rest of color values in the range of: (0.0031308, 1.0]
    // It uses the inverted general sRGB gamma curve which utilizes human eye brightness perception to
    //   encode lower precision in highlights (closer to 1) and higher precision in shadows (closer to 0).
    // If you plot the gamma curve on graph, you can then look at the "trajectory" of the curve:
    //   the steeper the angle of the curve is, the more "value space" the encoding allocates for this range.
    // The encoding algorithm can be simply explained by taking the input and output range value mappings:
    //   If we take values in the range of [0, 0.1], which are the color values for dark areas and shadows, then:
    //   - with Linear encoding, [0, 0.1] maps to [0, 0.1] - output value/point ends up
    //       exactly where the input value/point was.
    //     The input  value space for this range is: `(0.1 - 0) * 100 = 10.0%`.
    //     The output value space for this range is: `(0.1 - 0) * 100 = 10.0%`.
    //     They match exactly! There is no change of value space allocation.
	//   - with sRGB gamma power encoding, [0, 0.1] maps to [0, ~0.35] - output value/point ends up
	//       way more farther ahead compared to input.
	//     The input  value space for this range is: `(0.1  - 0) * 100 = 10.0%`.
	//     The output value space for this range is: `(0.35 - 0) * 100 = 35.0%`.
	//     So, for the same range of values the gamma encoding has `35 - 10 = 25%` more values
	//       than linear encoding to represent the same values.  That is where the increased precision comes from.
	//   If we take values in the range of [0.6, 1], which are the color values for bright areas and highlights, then:
	//   - with Linear encoding, [0.6, 1] maps to [0.6, 1] - no change, exactly as is.
	//     Input  value space: `(1 - 0.6) * 100 = 40.0%`.
	//     Output value space: `(1 - 0.6) * 100 = 40.0%`.
	//   - with sRGB gamma power encoding, [0.6, 1] maps to [~0.8, 1] - the output range became smaller.
	//     Input  value space: `(1 - 0.6) * 100 = 40.0%`.
	//     Output value space: `(1 - 0.8) * 100 = 20.0%`.
	//     As you can already tell, the gamma encoding provides `40 - 20 = 20%` less values
	//       than linear encoding.  So, with gamma encoding brighter values get less precision.
	// The "value space" concept might not be apparent at first because mathematically there is no limit in precision -
	//   you can have as much of the fractional digits as you want.  In the real world, however, there is no such luxury.
	//   For example, 32-bit float can store up to 15 fractional digits, but still, it cannot represent all values of that range.
	//   Then, think of the actual RGB 8-bit color where there are only 255 values per channel to represent the same we had in float.
	//   That is what it comes to and the difference it makes is gigantic.
    // `1.055 * x ^ ( 1 / 2.4 ) - 0.055`
    vec3 high = 1.055 * pow( linear, vec3( 1.0 / 2.4 ) ) - 0.055;
    // `cutoff` - a "switch" between two formulas:
    //   For `step( x, y )`:
    //   - If `x < y`, return 0.0
    //   - If `x >= y`, return 1.0
    vec3 cutoff = step( vec3( 0.0031308 ), linear );
    // Interpolate/"Mix" the calculated values using the `cutoff` weight/multiplier:
    //   For `mix( x, y, a )`, the computation formula is: `x * (1 - a)  +  y * a`, so:
    //   - If `cutoff = 0.0`, then `x * (1 - 0)  +  y * 0  =  1x + 0y  =  x`.  The function returns `low` value.
    //   - If `cutoff = 1.0`, then `x * (1 - 1)  +  y * 1  =  0x + 1y  =  y`.  The function returns `high` value.
    return mix( low, high, cutoff );
}

#define OREN_NAYAR_TRIGONOMETRIC 0

// Oren-Nayar diffuse shading / BRDF
// N - surface Normal direction vector, normalized.
// V - View direction vector, normalized.
// L - Light direction vector, normalized.
// roughness - material roughness, in [0, 1] (at this fragment; most commonly, from Roughness texture).
float OrenNayarDiffuse( vec3 N, vec3 V, vec3 L, float roughness ) {
	// float N_dot_L = max( 0.0, dot( N, L ) );  // How much does (N)ormal direction vector coincide with (L)ight vector.
	float N_dot_L = dot( N, L );  // How much does (N)ormal direction vector coincide with (L)ight vector.
	float N_dot_V = dot( N, V );  // How much does (N)ormal direction vector coincide with (V)iew vector.

	if ( N_dot_L < 0.0 ) {
		return 0.0;
	}

	// Calculate the angle between the view and light directions
	vec3 L_projected = normalize( L - N_dot_L * N );
	vec3 V_projected = normalize( V - N_dot_V * N );
	float cos_phi = dot( L_projected, V_projected );

#if OREN_NAYAR_TRIGONOMETRIC
	float theta_L = acos( N_dot_L );
	float theta_V = acos( N_dot_V );
	float sin_alpha = sin( max( theta_L, theta_V ) );
	float tan_beta = tan( min( theta_L, theta_V ) );
#else
	// For any angle θ, points on the unit circle satisfy:
	// 1. x = cos( θ )
	// 2. y = sin( θ )
	// 3. x^2 + y^2 = 1
	// Therefore:  cos^2( θ ) + sin^2( θ ) = 1
	// Both `dot( N, L )` and `dot( N, V )` produce cosine results -
	//   there is no explicit angle here; it is "baked" implicitly in
	//   a relationship between two vectors.
	//
	// This switch of angles prevents the approximation from falling apart:
	//   at grazing angles, while viewed from the light direction, the diffuse lighting
	//   produces brighter pixels while introducing black pixel areas at the same time.
	// It is an issue of computing at extremely low float values, which results in
	//   floating point precision errors.  This is exactly why and how we prevent this.
	float cos_larger = max( N_dot_L, N_dot_V );
	float cos_smaller = min( N_dot_L, N_dot_V );

	// sin( θ ) = sqrt( 1.0 - cos^2( θ ) )
	float sin_alpha = sqrt( 1.0  -  cos_smaller * cos_smaller );
	// tan( θ ) = sin( θ ) / cos( θ ) = sqrt( 1.0 - cos^2( θ ) )  /  cos( θ )
	// float tan_beta = sqrt( 1.0  -  cos_larger * cos_larger )  /  max( 1e-6, cos_larger );
	float tan_beta = sqrt( 1.0  -  cos_larger * cos_larger )  /  cos_larger;
#endif

	// Sigma: Roughness parameter (in radians: 0 = smooth, PI / 2 = very rough).
	// Map Roughness to `sigma`: [0, 1] -> [0, PI/2] or [0, ~1.57]
	// Since `sigma` is actually squared in the equations, it is best to square Roughness too to accommodate for that.
	// Then, the mapping will be perceptually linear.
	float sigma = ( roughness * roughness ) * PI_OVER_TWO;  // `roughness^2 * ( PI / 2.0 )`
	// float sigma = roughness * PI_OVER_TWO;
	float sigma_2 = sigma * sigma;

	// Oren-Nayar coefficients A and B.
	float A = 1.0 - 0.5 * ( sigma_2 / ( sigma_2 + 0.33 ) );
	float B = 0.45 * ( sigma_2 / ( sigma_2 + 0.09 ) );

	// The final Oren-Nayar diffuse term
	return N_dot_L * ( A + B * max( 0.0, cos_phi ) * sin_alpha * tan_beta );
}

// Lambertian diffuse shading / BRDF
// N - surface Normal direction vector, normalized.
// L - Light direction vector, normalized.
float LambertianDiffuse( vec3 N, vec3 L ) {
	float N_dot_L = dot( N, L );  // How much does (N)ormal direction vector coincide with (L)ight vector.
	// --- Old comment with explanation of dot product
	// `dot( N, L )` calculates how similar the vector directions are.
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
	// ---
	// `N_dot_L` being `< 0.0` means the (L)ight's direction points in the opposite way of (N)ormal,
	//   which in turn means the (L)ight is behind the surface.
	// To prevent this, we clamp the lower bound to 0.0 so the fragment does not get lit.
	return max( 0.0, N_dot_L );
}

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

		// Convert Specular value to Roughness.
		// They both describe the same physical property, just from different ends, so we can simply invert the value.
		float roughness = 1.0 - specular;
		float diffuse_term = OrenNayarDiffuse( normal, view_direction, light_direction, roughness );
		// float diffuse_term = LambertianDiffuse( normal, light_direction );

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
	// const float gamma = 2.2;
	// final_color = pow( final_color, vec3( 1.0 / gamma ) );
	final_color = LinearToSRGB( final_color );
	fragment_color = vec4( final_color.rgb, 1.0 );
}