#version 460 core
/*
	GLSL built-in vertex shader variables:

	in int gl_VertexID;
	in int gl_InstanceID;
	in int gl_DrawID;        // GLSL 4.60+ or ARB_shader_draw_parameters
	in int gl_BaseVertex;    // GLSL 4.60+ or ARB_shader_draw_parameters
	in int gl_BaseInstance;  // GLSL 4.60+ or ARB_shader_draw_parameters


	out gl_PerVertex
	{
		 vec4 gl_Position;
		float gl_PointSize;
		float gl_ClipDistance[];
	};
*/

layout (location = 0) in vec2 in_texture_uv;  // Vertex texture UV

// These are passed to fragment shader inputs.
// Variable names must be the same in both shaders.
// NOTE: Values get interpolated using barycentric coordinates for triangles (triangle-space coordinates)
// on Rasterization stage (after Vertex shader, before Fragment shader; fixed function, non-programmable).
out vec2 fragment_texture_uv;  // Fragment interpolated texture UV (no transformations needed)

void main()
{
	fragment_texture_uv = in_texture_uv;

	vec2 ndc_position = ( in_texture_uv - 0.5 ) * 2; // [0; 1] -> [-1; 1]
	gl_Position = vec4( ndc_position, 0.0, 1.0 );
}