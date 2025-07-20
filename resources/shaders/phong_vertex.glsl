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
out vec3 fragment_position;    // Fragment interpolated World-space position
out vec3 fragment_normal;      // Fragment interpolated World-space normal
out vec2 fragment_texture_uv;  // Fragment interpolated texture UV (no transformations needed)

// uniform mat4 model;       // Local (Object) -> World space
// uniform mat4 view;        // World -> View (Camera/Eye) space
// uniform mat4 projection;  // View -> Clip space [-1.0; 1.0] (-> Screen space [1920; 1080])

void main()
{
	// fragment_position = vec3(model * vec4(in_position, 1.0));
	// Do this on CPU !!!
	// fragment_normal = mat3(transpose(inverse(model))) * in_normal;
	fragment_texture_uv = in_texture_uv;

	// gl_Position = projection * view * model * vec4(in_position, 1.0);
	gl_Position = vec4( in_texture_uv.xy, 0.0, 1.0 );
}