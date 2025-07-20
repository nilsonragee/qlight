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

layout (location = 0) in vec3 in_position;    // Vertex Local-space position
layout (location = 1) in vec3 in_normal;      // Vertex Local-space normal
layout (location = 2) in vec2 in_texture_uv;  // Vertex texture UV

// These are passed to fragment shader inputs.
// Variable names must be the same in both shaders.
// NOTE: Values get interpolated using barycentric coordinates for triangles (triangle-space coordinates)
// on Rasterization stage (after Vertex shader, before Fragment shader; fixed function, non-programmable).
out vec3 fragment_position;    // Fragment interpolated World-space position
out vec3 fragment_normal;      // Fragment interpolated World-space normal
out vec2 fragment_texture_uv;  // Fragment interpolated texture UV (no transformations needed)

// Per-mesh
uniform mat4 model;          // Local (Object) -> World space
uniform mat3 normal_matrix;  // mat3( transpose( inverse( model ) ) ) - Correction matrix for non-uniform scaling

// Per-material
uniform mat4 view;           // World -> View (Camera/Eye) space
uniform mat4 projection;     // View -> Clip space [-1.0; 1.0] (-> Screen space [1920; 1080])

void main()
{
	// Transform: Local space -> World space position.
	vec4 position_xyzw = vec4( in_position, 1.0 );
	position_xyzw = model * position_xyzw;
	// fragment_position = model * vec4( in_position.xyz, 1.0 );
	fragment_position = position_xyzw.xyz;

	// Transform: Local space -> World space normal vector (corrected for non-uniform scaling)
	fragment_normal = normal_matrix * in_normal;

	// Pass as is: no transformations needed.
	fragment_texture_uv = in_texture_uv;

	// ... * view * position_xyzw// `* model` is already applied.
	gl_Position = projection * view * model * vec4( in_position, 1.0 );
	// gl_Position = vec4(0.0, 0.0, 0.5, 1.0);
}