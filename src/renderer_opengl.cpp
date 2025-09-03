#define _CRT_SECURE_NO_WARNINGS // @TODO: Remove
#include "renderer.h"
#include "texture.h"

#define QL_LOG_CHANNEL "Renderer"
#include "log.h"

constexpr u64 RENDERER_INITIAL_PROGRAMS_CAPACITY = 8;
constexpr u64 RENDERER_INITIAL_STAGES_CAPACITY = 16;
constexpr u64 RENDERER_INITIAL_FRAMEBUFFERS_CAPACITY = 3;
constexpr u64 RENDERER_INITIAL_RENDERBUFFERS_CAPACITY = 4;
constexpr u64 RENDERER_INITIAL_UNIFORM_BUFFERS_CAPACITY = 12;

constexpr u64 RENDERER_INITIAL_FRAMEBUFFER_ATTACHMENTS_CAPACITY = 16;

constexpr u64 RENDERER_OPENGL_ERROR_LOG_CAPACITY = 4096;
constexpr u64 RENDERER_OPENGL_INFO_LOG_CAPACITY = 4096;

struct Geometry_Buffer {
	Renderer_Framebuffer_ID framebuffer;
	Renderer_Shader_Program *shader_program;
	Texture_ID texture_position;
	Texture_ID texture_normal;
	Texture_ID texture_color_specular;  // 24-bit color, 8-bit specular combined
	Renderer_Renderbuffer_ID renderbuffer_depth_stencil;  // 24-bit depth, 8-bit stencil combined
	Vector2_u16 dimensions;
};

struct GL_Constants {
	u32 max_color_attachments;
	u32 max_uniform_block_size;
	u32 max_uniform_locations;
	u32 max_uniform_buffer_bindings;
	u32 min_map_buffer_alignment;
};

struct G_Renderer {
	struct Frame_Time {
		f32 last;
		f32 current;
		f32 delta;
	} frame_time;

	struct Device {
		StringView_ASCII vendor;
		StringView_ASCII name;
	} device;

	Array< Renderer_Framebuffer > framebuffers;
	Array< Renderer_Renderbuffer > renderbuffers;
	Array< Renderer_Shader_Program > programs;
	Array< Renderer_Shader_Stage > stages;
	Array< Renderer_Uniform_Buffer > uniform_buffers;

	Geometry_Buffer gbuffer;

	Vector3_f32 *camera_position;
	Matrix4x4_f32 *view_matrix;
	Matrix4x4_f32 *projection_matrix;
	Vector3_f32 ambient_light;

	Vector4_f32 clear_color;
	Array< Renderer_Render_Command > render_queue;
	// How many consecutive sorted render queue commands have same material.
	Array< u16 > render_queue_material_sequence;

	Texture_ID texture_white;
	Texture_ID texture_black;
	Texture_ID texture_purple_checkers;

	Mesh_ID fullscreen_quad;

	Renderer_Output_Channel output_channel;

	// OpenGL-specific:
	GL_Constants gl_constants;
	/*
		OpenGL definition of glUniformMatrixNxM: 'The first number in the
			command name is the number of columns; the second is the number of rows.'
	*/
	bool uniforms_transpose_matrix; // [columns] x [rows] -> [rows] x [columns]
	String_ASCII opengl_error_log;
	String_ASCII opengl_info_log;

} g_renderer;

// Fullscreen quad attributes
struct Vertex_Quad {
	Vector2_f32 texture_uv;
};

#define QLIGHT_OPENGL_ERROR_CHECKS

#ifdef QLIGHT_OPENGL_ERROR_CHECKS
#define GL_CLEAR_LOG_AND_EXECUTE( expression )  \
	opengl_error_clear();  \
	expression

#define GL_CHECK( expression )  \
	GL_CLEAR_LOG_AND_EXECUTE( expression );  \
	opengl_error_log( #expression, __FILE__, __LINE__ )

#define GL_CHECK_AND_STORE_RESULT( result_pointer, expression )  \
	GL_CLEAR_LOG_AND_EXECUTE( expression );  \
	*result_pointer = opengl_error_log( #expression, __FILE__, __LINE__ )

#define GL_ASSERT( expression )  \
	GL_CLEAR_LOG_AND_EXECUTE( expression );  \
	bool gl_call_generated_error = opengl_error_log( #expression, __FILE__, __LINE__ );  \
	AssertMessage( gl_call_generated_error == GL_TRUE, #expression )

#else /* QLIGHT_OPENGL_ERROR_CHECKS */
#define GL_CLEAR_LOG_AND_EXECUTE( expression )  expression
#define GL_CHECK( expression )  expression
#define GL_CHECK_AND_STORE_RESULT( result_pointer, expression )  expression
#define GL_ASSERT( expression )  expression
#endif


#define log_gl( log_level, format, ... )  log( log_level, QL_LOG_CHANNEL "/GL", format, __VA_ARGS__ )
#define log_error_gl( format, ... )  log_gl( LogLevel_Error, format, __VA_ARGS__ )
#define log_warning_gl( format, ... )  log_gl( LogLevel_Warning, format, __VA_ARGS__ )
#ifdef QLIGHT_DEBUG
#define log_debug_gl( format, ... )  log_gl( LogLevel_Debug, format, __VA_ARGS__ )
#else
#define log_debug_gl( format, ... )
#endif

static void
opengl_error_clear() {
    while ( glGetError() != GL_NO_ERROR ); // or !glGetError()
}

static bool
opengl_error_log( const char *function, const char *file, int line ) {
    while ( GLenum error = glGetError() ) {
		log_error_gl( "OpenGL Error #%u generated at:\n%s:%d: %s",
			error,
			file,
			line,
			function
		);
        return false;
    }
    return true;
}

enum OpenGL_Object_Type : u32 {
	OpenGL_Shader = GL_SHADER,
	OpenGL_Program = GL_PROGRAM,
	OpenGL_ProgramPipeline = GL_PROGRAM_PIPELINE
};

static StringView_ASCII
opengl_get_info_log( OpenGL_Object_Type object, GLuint id ) {
	GLint log_length;
	// string_clear( g_renderer.opengl_info_log );

	switch ( object ) {
		case OpenGL_Shader:
			glGetShaderiv( id, GL_INFO_LOG_LENGTH, &log_length );
			glGetShaderInfoLog( id, log_length, &log_length, g_renderer.opengl_info_log.data );
			break;
		case OpenGL_Program:
			glGetProgramiv( id, GL_INFO_LOG_LENGTH, &log_length );
			glGetProgramInfoLog( id, log_length, &log_length, g_renderer.opengl_info_log.data );
			break;
		case OpenGL_ProgramPipeline:
			glGetProgramPipelineiv( id, GL_INFO_LOG_LENGTH, &log_length );
			glGetProgramPipelineInfoLog( id, log_length, &log_length, g_renderer.opengl_info_log.data );
			break;
		default:
			return string_view( ( const char * )NULL );
	}

	g_renderer.opengl_info_log.size = log_length;

	return string_view( &g_renderer.opengl_info_log );
}

static void
opengl_generate_and_bind_vertex_array( GLuint *id, StringView_ASCII debug_name ) {
	glGenVertexArrays( 1, id );
	glBindVertexArray( *id );
#ifdef QLIGHT_DEBUG
	if ( debug_name.size > 0 )
		glObjectLabel( GL_VERTEX_ARRAY, *id, debug_name.size, debug_name.data );
#endif
	log_debug_gl( "Generated and bound VAO '" StringViewFormat "' (gl_id: %u).",
		StringViewArgument( debug_name ),
		*id
	);
}

static void
opengl_create_and_bind_vertex_array( GLuint *id, StringView_ASCII debug_name ) {
	glCreateVertexArrays( 1, id );
	glBindVertexArray( *id );
#ifdef QLIGHT_DEBUG
	if ( debug_name.size > 0 )
		glObjectLabel( GL_VERTEX_ARRAY, *id, debug_name.size, debug_name.data );
#endif
	log_debug_gl( "Created and bound VAO '" StringViewFormat "' (gl_id: %u).",
		StringViewArgument( debug_name ),
		*id
	);
}

static void
opengl_generate_and_bind_vertex_buffer( GLuint *id, StringView_ASCII debug_name ) {
	glGenBuffers( 1, id );
	glBindBuffer( GL_ARRAY_BUFFER, *id );
#ifdef QLIGHT_DEBUG
	if ( debug_name.size > 0 )
		glObjectLabel( GL_BUFFER, *id, debug_name.size, debug_name.data );
#endif
	log_debug_gl( "Generated and bound VBO '" StringViewFormat "' (gl_id: %u).",
		StringViewArgument( debug_name ),
		*id
	);
}

static void
opengl_create_and_bind_vertex_buffer( GLuint *id, StringView_ASCII debug_name ) {
	glCreateBuffers( 1, id );
	glBindBuffer( GL_ARRAY_BUFFER, *id );
#ifdef QLIGHT_DEBUG
	if ( debug_name.size > 0 )
		glObjectLabel( GL_BUFFER, *id, debug_name.size, debug_name.data );
#endif
	log_debug_gl( "Created and bound VBO '" StringViewFormat "' (gl_id: %u).",
		StringViewArgument( debug_name ),
		*id
	);
}

static void
opengl_generate_and_bind_element_buffer( GLuint *id, StringView_ASCII debug_name ) {
	glGenBuffers( 1, id );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, *id );
#ifdef QLIGHT_DEBUG
	if ( debug_name.size > 0 )
		glObjectLabel( GL_BUFFER, *id, debug_name.size, debug_name.data );
#endif
	log_debug_gl( "Generated and bound EBO '" StringViewFormat "' (gl_id: %u).",
		StringViewArgument( debug_name ),
		*id
	);
}

static void
opengl_create_and_bind_element_buffer( GLuint *id, StringView_ASCII debug_name ) {
	glCreateBuffers( 1, id );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, *id );
#ifdef QLIGHT_DEBUG
	if ( debug_name.size > 0 )
		glObjectLabel( GL_BUFFER, *id, debug_name.size, debug_name.data );
#endif
	log_debug_gl( "Created and bound EBO '" StringViewFormat "' (gl_id: %u).",
		StringViewArgument( debug_name ),
		*id
	);
}

static GLenum
index_type_size_to_opengl( u32 size ) {
	switch ( size ) {
		case 1: return GL_UNSIGNED_BYTE;
		case 2: return GL_UNSIGNED_SHORT;
		case 4: return GL_UNSIGNED_INT;
		default: return 0;
	}
}

static StringView_ASCII
opengl_storage_format_name( GLint internal_format ) {
	switch ( internal_format ) {
		// Base Internal Formats
		case GL_DEPTH_COMPONENT: return "GL_DEPTH_COMPONENT";
		case GL_DEPTH_STENCIL: return "GL_DEPTH_STENCIL";
		case GL_RED: return "GL_RED";
		case GL_RG: return "GL_RG";
		case GL_RGB: return "GL_RGB";
		case GL_RGBA: return "GL_RGBA";

		// Sized Internal Formats
		case GL_R8: return "GL_R9";
		case GL_R8_SNORM: return "GL_R8_SNORM";
		case GL_R16: return "GL_R16";
		case GL_R16_SNORM: return "GL_R16_SNORM";
		case GL_RG8: return "GL_RG9";
		case GL_RG8_SNORM: return "GL_RG8_SNORM";
		case GL_RG16: return "GL_RG16";
		case GL_RG16_SNORM: return "GL_RG16_SNORM";
		case GL_R3_G3_B2: return "GL_R3_G3_B2";
		case GL_RGB4: return "GL_RGB4";
		case GL_RGB5: return "GL_RGB5";
		case GL_RGB8: return "GL_RGB8";
		case GL_RGB8_SNORM: return "GL_RGB8_SNORM";
		case GL_RGB10: return "GL_RGB10";
		case GL_RGB12: return "GL_RGB12";
		case GL_RGB16_SNORM: return "GL_RGB16_SNORM";
		case GL_RGBA2: return "GL_RGBA2";
		case GL_RGBA4: return "GL_RGBA4";
		case GL_RGB5_A1: return "GL_RGB5_A1";
		case GL_RGBA8: return "GL_RGBA8";
		case GL_RGBA8_SNORM: return "GL_RGBA8_SNORM";
		case GL_RGB10_A2: return "GL_RGB10_A2";
		case GL_RGB10_A2UI: return "GL_RGB10_A2UI";
		case GL_RGBA12: return "GL_RGBA12";
		case GL_RGBA16: return "GL_RGBA16";
		case GL_SRGB8: return "GL_SRGB8";
		case GL_SRGB8_ALPHA8: return "GL_SRGB8_ALPHA8";
		case GL_R16F: return "GL_R16F";
		case GL_RG16F: return "GL_RG16F";
		case GL_RGB16F: return "GL_RGB16F";
		case GL_RGBA16F: return "GL_RGBA16F";
		case GL_R32F: return "GL_R32F";
		case GL_RG32F: return "GL_RG32F";
		case GL_RGB32F: return "GL_RGB32F";
		case GL_RGBA32F: return "GL_RGBA32F";
		case GL_R11F_G11F_B10F: return "GL_R11F_G11F_B10F";
		case GL_RGB9_E5: return "GL_RGB9_E5";
		case GL_R8I: return "GL_R8I";
		case GL_R8UI: return "GL_R8UI";
		case GL_R16I: return "GL_R16I";
		case GL_R16UI: return "GL_R16UI";
		case GL_R32I: return "GL_R32I";
		case GL_R32UI: return "GL_R32UI";
		case GL_RG8I: return "GL_RG8I";
		case GL_RG8UI: return "GL_RG8UI";
		case GL_RG16I: return "GL_RG16I";
		case GL_RG16UI: return "GL_RG16UI";
		case GL_RG32I: return "GL_RG32I";
		case GL_RG32UI: return "GL_RG32UI";
		case GL_RGB8I: return "GL_RGB8I";
		case GL_RGB8UI: return "GL_RGB8UI";
		case GL_RGB16I: return "GL_RGB16I";
		case GL_RGB16UI: return "GL_RGB16UI";
		case GL_RGB32I: return "GL_RGB32I";
		case GL_RGB32UI: return "GL_RGB32UI";
		case GL_RGBA8I: return "GL_RGBA8I";
		case GL_RGBA8UI: return "GL_RGBA8UI";
		case GL_RGBA16I: return "GL_RGBA16I";
		case GL_RGBA16UI: return "GL_RGBA16UI";
		case GL_RGBA32I: return "GL_RGBA32I";
		case GL_RGBA32UI: return "GL_RGBA32UI";

		// Compressed Internal Formats
		case GL_COMPRESSED_RED: return "GL_COMPRESSED_RED";
		case GL_COMPRESSED_RG: return "GL_COMPRESSED_RG";
		case GL_COMPRESSED_RGB: return "GL_COMPRESSED_RGB";
		case GL_COMPRESSED_RGBA: return "GL_COMPRESSED_RGBA";
		case GL_COMPRESSED_SRGB: return "GL_COMPRESSED_SRGB";
		case GL_COMPRESSED_SRGB_ALPHA: return "GL_COMPRESSED_SRGB_ALPHA";
		case GL_COMPRESSED_RED_RGTC1: return "GL_COMPRESSED_RED_RGTC1";
		case GL_COMPRESSED_SIGNED_RED_RGTC1: return "GL_COMPRESSED_SIGNED_RED_RGTC1";
		case GL_COMPRESSED_RG_RGTC2: return "GL_COMPRESSED_RG_RGTC2";
		case GL_COMPRESSED_SIGNED_RG_RGTC2: return "GL_COMPRESSED_SIGNED_RG_RGTC2";
		case GL_COMPRESSED_RGBA_BPTC_UNORM: return "GL_COMPRESSED_RGBA_BPTC_UNORM";
		case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM: return "GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM";
		case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT: return "GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT";
		case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT: return "GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT";

		case GL_DEPTH24_STENCIL8: return "GL_DEPTH24_STENCIL8";

		default: return "(unknown)";
	}
}

static StringView_ASCII
opengl_pixel_type_name( GLenum pixel_type ) {
	switch ( pixel_type ) {
		// 1 byte
		case GL_UNSIGNED_BYTE: return  "GL_UNSIGNED_BYTE";
		case GL_BYTE:  return "GL_BYTE";

		case GL_UNSIGNED_BYTE_3_3_2:  return "GL_UNSIGNED_BYTE_3_3_2";
		case GL_UNSIGNED_BYTE_2_3_3_REV:  return "GL_UNSIGNED_BYTE_2_3_3_REV";

		// 2 bytes
		case GL_UNSIGNED_SHORT:  return "GL_UNSIGNED_SHORT";
		case GL_SHORT:  return "GL_SHORT";

		case GL_UNSIGNED_SHORT_5_6_5:  return "GL_UNSIGNED_SHORT_5_6_5";
		case GL_UNSIGNED_SHORT_5_6_5_REV:  return "GL_UNSIGNED_SHORT_5_6_5_REV";
		case GL_UNSIGNED_SHORT_4_4_4_4:  return "GL_UNSIGNED_SHORT_4_4_4_4";
		case GL_UNSIGNED_SHORT_4_4_4_4_REV:  return "GL_UNSIGNED_SHORT_4_4_4_4_REV";
		case GL_UNSIGNED_SHORT_5_5_5_1:  return "GL_UNSIGNED_SHORT_5_5_5_1";
		case GL_UNSIGNED_SHORT_1_5_5_5_REV:  return "GL_UNSIGNED_SHORT_1_5_5_5_REV";

		// 4 bytes
		case GL_UNSIGNED_INT:  return "GL_UNSIGNED_INT";
		case GL_INT:  return "GL_INT";
		case GL_FLOAT:  return "GL_FLOAT";

		case GL_UNSIGNED_INT_8_8_8_8:  return "GL_UNSIGNED_INT_8_8_8_8";
		case GL_UNSIGNED_INT_8_8_8_8_REV:  return "GL_UNSIGNED_INT_8_8_8_8_REV";
		case GL_UNSIGNED_INT_10_10_10_2:  return "GL_UNSIGNED_INT_10_10_10_2";
		case GL_UNSIGNED_INT_2_10_10_10_REV:  return "GL_UNSIGNED_INT_2_10_10_10_REV";

		default:  return "(unknown)";
	}
}

static void
create_default_textures() {
	log_debug( "Creating default textures..." );

	/* White Texture */

	u8 white_bytes[] = { /* R */ 255, /* G */ 255, /* B */ 255, /* A */ 255 };
	ArrayView< u8 > white_bytes_view = array_view< u8 >( white_bytes, ARRAY_SIZE( white_bytes ) );
	Texture_ID white_texture_id = texture_create(
		/*       name */ "White Texture",
		/* dimensions */ { 1, 1 },
		/*   channels */ TextureChannels_RGBA,
		/*      bytes */ white_bytes_view,
		/*  allocator */ NULL
	);
	Texture *white_texture = texture_instance( white_texture_id );
	renderer_texture_2d_upload(
		/*            texture_id */ white_texture_id,
		/*                origin */ { 0, 0 },
		/*            dimensions */ white_texture->dimensions,
		/*         mipmap_levels */ 1,
		/* opengl_storage_format */ GL_RGBA8,
		/*     opengl_pixel_type */ GL_UNSIGNED_BYTE
	);
	g_renderer.texture_white = white_texture_id;

	/* Black Texture */

	u8 black_bytes[] = { /* R */ 0, /* G */ 0, /* B */ 0, /* A */ 255 };
	ArrayView< u8 > black_bytes_view = array_view< u8 >( black_bytes, ARRAY_SIZE( black_bytes ) );
	Texture_ID black_texture_id = texture_create(
		/*       name */ "Black Texture",
		/* dimensions */ { 1, 1 },
		/*   channels */ TextureChannels_RGBA,
		/*      bytes */ black_bytes_view,
		/*  allocator */ NULL
	);
	Texture *black_texture = texture_instance( black_texture_id );
	renderer_texture_2d_upload(
		/*            texture_id */ black_texture_id,
		/*                origin */ { 0, 0 },
		/*            dimensions */ black_texture->dimensions,
		/*         mipmap_levels */ 1,
		/* opengl_storage_format */ GL_RGBA8,
		/*     opengl_pixel_type */ GL_UNSIGNED_BYTE
	);
	g_renderer.texture_black = black_texture_id;

	/* Purple Checkboard Texture */

	u8 checkboard_bytes[] = {
		// R    G    B    A
		   130, 0,   200, 255,  // (purple pixel)
		   0,   0,   0,   255,  // ( black pixel)
		   0,   0,   0,   255,  // ( black pixel)
		   130, 0,   200, 255   // (purple pixel)
	};
	ArrayView< u8 > checkboard_view = array_view< u8 >( checkboard_bytes, ARRAY_SIZE( checkboard_bytes ) );
	Texture_ID checkboard_texture_id = texture_create(
		/*       name */ "Purple Checkboard Texture",
		/* dimensions */ { 2, 2 },
		/*   channels */ TextureChannels_RGBA,
		/*      bytes */ checkboard_view,
		/*  allocator */ NULL
	);
	Texture *checkboard_texture = texture_instance( checkboard_texture_id );
	renderer_texture_2d_upload(
		/*            texture_id */ checkboard_texture_id,
		/*                origin */ { 0, 0 },
		/*            dimensions */ checkboard_texture->dimensions,
		/*         mipmap_levels */ 1,
		/* opengl_storage_format */ GL_RGBA8,
		/*     opengl_pixel_type */ GL_UNSIGNED_BYTE
	);
	g_renderer.texture_purple_checkers = checkboard_texture_id;

	log_debug( "Default textures have been created." );
}

static void
setup_fullscreen_quad() {
	log_debug( "Setting up Fullscreen Quad..." );
	Array< Renderer_Vertex_Attribute > attributes = array_new< Renderer_Vertex_Attribute >( sys_allocator, 1 );

	array_add( &attributes, Renderer_Vertex_Attribute {
		.name = "texture_uv",
		.index = 0,
		.elements = 2,
		.data_type = RendererDataType_f32,
		.normalize = false,
		.active = true
	} );

	Mesh quad_mesh = {
		.name = string_new( sys_allocator, "Fullscreen Quad" ),
		// .vertices = array_new< Vertex_Quad >( sys_allocator, 4 ),
		// .indices = array_new< u32 >( sys_allocator, 3 * 2 ),  // 3 vertices per 2 triangles
		.material_id = INVALID_MATERIAL_ID,
		.vertex_attributes = attributes
	};

	u32 vertex_size = mesh_vertex_attributes_size( &quad_mesh );
	quad_mesh.vertices = carray_new( sys_allocator, vertex_size, 4 );
	u32 index_type_size = sizeof( u16 );
	quad_mesh.indices = carray_new( sys_allocator, index_type_size, 3 * 2 );  // 3 vertices per 2 triangles

	Vertex_Quad quad_vertices[] = {
		{ .texture_uv = Vector2_f32( 0.0f, 0.0f ) },  // Bottom-left
		{ .texture_uv = Vector2_f32( 0.0f, 1.0f ) },  // Bottom-right
		{ .texture_uv = Vector2_f32( 1.0f, 1.0f ) },  // Top-right
		{ .texture_uv = Vector2_f32( 1.0f, 0.0f ) }   // Top-left
	};
	CArrayView vertices_view = carray_view_create(
		/*      size */ ARRAY_SIZE( quad_vertices ),
		/* item_size */ vertex_size,
		/*      data */ quad_vertices
	);

	carray_add_many( &quad_mesh.vertices, vertices_view );

	u16 quad_indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	CArrayView indices_view = carray_view_create(
		/*      size */ ARRAY_SIZE( quad_indices ),
		/* item_size */ index_type_size,
		/*      data */ quad_indices
	);

	carray_add_many( &quad_mesh.indices, indices_view );

	Mesh_ID mesh_id = mesh_store( &quad_mesh );
	renderer_mesh_upload( mesh_id );
	g_renderer.fullscreen_quad = mesh_id;
	log_debug( "Fullscreen Quad has been set up." );
}

static void
draw_fullscreen_quad() {
	Mesh *mesh = mesh_instance( g_renderer.fullscreen_quad );
	glBindVertexArray( mesh->opengl_vao );
	GLenum index_type = index_type_size_to_opengl( mesh->indices.item_size );
	glDrawElements(
		/*    mode */ GL_TRIANGLES,
		/*   count */ mesh->indices.size,
		/*    type */ index_type,
		/* indices */ NULL
	);
}

Renderer_Shader_Program *
load_geometry_buffer_shader( StringView_ASCII vertex_stage_filepath, StringView_ASCII fragment_stage_filepath ) {

	/* Stages */

	Array< Renderer_Shader_Stage * > stages = array_new< Renderer_Shader_Stage * >( sys_allocator, RendererShaderKind_COUNT );

	// Vertex
	array_add( &stages, renderer_load_shader_stage(
		"gbuffer_vertex",
		RendererShaderKind_Vertex,
		vertex_stage_filepath
	) );

	// Fragment
	array_add( &stages, renderer_load_shader_stage(
		"gbuffer_fragment",
		RendererShaderKind_Fragment,
		fragment_stage_filepath
	) );

	Renderer_Shader_Program *shader = renderer_create_and_compile_shader_program(
		"gbuffer_program",
		array_view( &stages )
	);

	/* Attributes */

	Array< Renderer_Vertex_Attribute > *attributes = &shader->vertex_attributes;

	array_add( attributes, Renderer_Vertex_Attribute {
		.name = "position",
		.index = 0,
		.elements = 3,
		.data_type = RendererDataType_f32,
		.normalize = false,
		.active = true
	} );

	array_add( attributes, Renderer_Vertex_Attribute {
		.name = "normal",
		.index = 1,
		.elements = 3,
		.data_type = RendererDataType_f32,
		.normalize = false,
		.active = true
	} );

	array_add( attributes, Renderer_Vertex_Attribute {
		.name = "texture_uv",
		.index = 2,
		.elements = 2,
		.data_type = RendererDataType_f32,
		.normalize = false,
		.active = true
	} );

	/* Uniforms */

	Array< Renderer_Uniform > *uniforms = &shader->uniforms;

	/* Vertex stage uniforms */

	array_add( uniforms, Renderer_Uniform {
		.name = "model",
		.data_type = RendererDataType_Matrix4x4_f32,
	} );

	array_add( uniforms, Renderer_Uniform {
		.name = "view",
		.data_type = RendererDataType_Matrix4x4_f32,
	} );

	array_add( uniforms, Renderer_Uniform {
		.name = "projection",
		.data_type = RendererDataType_Matrix4x4_f32,
	} );

	array_add( uniforms, Renderer_Uniform {
		.name = "normal_matrix",
		.data_type = RendererDataType_Matrix3x3_f32,
	} );

	/* Fragment stage uniforms */

	array_add( uniforms, Renderer_Uniform {
		.name = "texture_diffuse0",
		.data_type = RendererDataType_s32, // Sampler2D
	} );

	array_add( uniforms, Renderer_Uniform {
		.name = "texture_normal0",
		.data_type = RendererDataType_s32, // Sampler2D
	} );

	array_add( uniforms, Renderer_Uniform {
		.name = "texture_specular0",
		.data_type = RendererDataType_s32, // Sampler2D
	} );

	renderer_shader_program_update_uniform_locations( shader );
	return shader;
}

// TODO: recreate_geometry_buffer; resize existing attachment textures
static void
setup_geometry_buffer( Vector2_u16 framebuffer_dimensions ) {
	log_debug( "Setting up Geometry Buffer..." );
	g_renderer.gbuffer.framebuffer = renderer_create_framebuffer( "gbuffer_framebuffer" );
	g_renderer.gbuffer.dimensions = framebuffer_dimensions;

	g_renderer.gbuffer.shader_program = load_geometry_buffer_shader(
		"resources/shaders/geometry_vertex.glsl",
		"resources/shaders/geometry_fragment.glsl"
	);

	/* G-Buffer Position texture */

	g_renderer.gbuffer.texture_position = texture_create(
		/*       name */ "gbuffer_position",
		/* dimensions */ framebuffer_dimensions,
		/*   channels */ TextureChannels_RGB,
		/*      bytes */ ArrayView< u8 > {},
		/*  allocator */ NULL
	);
	renderer_texture_2d_upload(
		/*             texture_id */ g_renderer.gbuffer.texture_position,
		/*                origin */ { 0, 0 },
		/*            dimensions */ framebuffer_dimensions,
		/*         mipmap_levels */ 1,
		/* opengl_storage_format */ GL_RGB16F,
		/*    opengl_pixel_type  */ GL_FLOAT
	);
	renderer_texture_attach_to_framebuffer(
		g_renderer.gbuffer.texture_position,
		g_renderer.gbuffer.framebuffer,
		RendererFramebufferAttachmentPoint_Color0
	);

	/* G-Buffer Normal texture */

	g_renderer.gbuffer.texture_normal = texture_create(
		/*       name */ "gbuffer_normal",
		/* dimensions */ framebuffer_dimensions,
		/*   channels */ TextureChannels_RGB,
		/*      bytes */ ArrayView< u8 > {},
		/*  allocator */ NULL
	);
	renderer_texture_2d_upload(
		/*            texture_id */ g_renderer.gbuffer.texture_normal,
		/*                origin */ { 0, 0 },
		/*            dimensions */ framebuffer_dimensions,
		/*         mipmap_levels */ 1,
		/* opengl_storage_format */ GL_RGB16F,
		/*    opengl_pixel_type  */ GL_FLOAT
	);
	renderer_texture_attach_to_framebuffer(
		g_renderer.gbuffer.texture_normal,
		g_renderer.gbuffer.framebuffer,
		RendererFramebufferAttachmentPoint_Color1
	);

	/* G-Buffer Color/Specular texture */

	g_renderer.gbuffer.texture_color_specular = texture_create(
		/*       name */ "gbuffer_color_specular",
		/* dimensions */ framebuffer_dimensions,
		/*   channels */ TextureChannels_RGBA,
		/*      bytes */ ArrayView< u8 > {},
		/*  allocator */ NULL
	);
	renderer_texture_2d_upload(
		/*            texture_id */ g_renderer.gbuffer.texture_color_specular,
		/*                origin */ { 0, 0 },
		/*            dimensions */ framebuffer_dimensions,
		/*         mipmap_levels */ 1,
		/* opengl_storage_format */ GL_RGBA8,
		/*    opengl_pixel_type  */ GL_UNSIGNED_BYTE
	);
	renderer_texture_attach_to_framebuffer(
		g_renderer.gbuffer.texture_color_specular,
		g_renderer.gbuffer.framebuffer,
		RendererFramebufferAttachmentPoint_Color2
	);

	Renderer_Framebuffer_Attachment_Point active_attachment_points[] = {
		RendererFramebufferAttachmentPoint_Color0,  // position
		RendererFramebufferAttachmentPoint_Color1,  // normal
		RendererFramebufferAttachmentPoint_Color2   // color + specular
	};

	renderer_set_active_framebuffer_color_attachment_points(
		g_renderer.gbuffer.framebuffer,
		array_view( active_attachment_points, ARRAY_SIZE( active_attachment_points ) )
	);

	/* G-Buffer Depth/Stencil Renderbuffer */

	g_renderer.gbuffer.renderbuffer_depth_stencil = renderer_create_renderbuffer(
		/*                  name */"gbuffer_depth_stencil",
		/*            dimensions */ framebuffer_dimensions,
		/*      attachment_point */ RendererFramebufferAttachmentPoint_DepthStencil,
		/* opengl_storage_format */ GL_DEPTH24_STENCIL8
	);
	renderer_renderbuffer_attach_to_framebuffer(
		g_renderer.gbuffer.framebuffer,
		g_renderer.gbuffer.renderbuffer_depth_stencil,
		RendererFramebufferAttachmentPoint_DepthStencil
	);

	log_debug( "Geometry Buffer has been set up." );
}

static void
renderer_create_default_framebuffer() {
	// It does not create a new one because framebuffer #0 is already provided by OpenGL.
	Renderer_Framebuffer default_framebuffer = {
		.name = "default",
		.opengl_framebuffer = 0
	};

	u32 framebuffer_idx = array_add( &g_renderer.framebuffers, default_framebuffer );

	renderer_bind_framebuffer( 0 );
}

static StringView_ASCII
opengl_debug_message_source_name( GLenum source ) {
	switch ( source ) {
		case GL_DEBUG_SOURCE_API:              return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:    return "Window System";
		case GL_DEBUG_SOURCE_SHADER_COMPILER:  return "Shader Compiler";
		case GL_DEBUG_SOURCE_THIRD_PARTY:      return "Third Party";
		case GL_DEBUG_SOURCE_APPLICATION:      return "Application";

		case GL_DEBUG_SOURCE_OTHER:
		default:                               return "Other";
	}
}

static StringView_ASCII
opengl_debug_message_type_name( GLenum type ) {
	switch ( type ) {
		case GL_DEBUG_TYPE_ERROR:                return "Error";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:  return "Deprecated";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:   return "Undefined";
		case GL_DEBUG_TYPE_PORTABILITY:          return "Portability";
		case GL_DEBUG_TYPE_PERFORMANCE:          return "Performance";
		case GL_DEBUG_TYPE_MARKER:               return "Marker";
		case GL_DEBUG_TYPE_PUSH_GROUP:           return "Push Group";
		case GL_DEBUG_TYPE_POP_GROUP:            return "Pop Group";

		case GL_DEBUG_TYPE_OTHER:
		default:                                 return "Other";
	}
}

static StringView_ASCII
opengl_debug_message_severity_name( GLenum severity ) {
	switch ( severity ) {
		case GL_DEBUG_SEVERITY_HIGH:          return "High";
		case GL_DEBUG_SEVERITY_MEDIUM:        return "Medium";
		case GL_DEBUG_SEVERITY_LOW:           return "Low";
		case GL_DEBUG_SEVERITY_NOTIFICATION:  return "Info";

		default:                              return "(Unknown)";
	}
}

static void
opengl_debug_message_callback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *message,
	const void *user_parameter
) {
	StringView_ASCII msg = string_view( message, /* offset */ 0, /* length */ length );
	StringView_ASCII source_name = opengl_debug_message_source_name( source );
	StringView_ASCII type_name = opengl_debug_message_type_name( type );
	StringView_ASCII severity_name = opengl_debug_message_severity_name( severity );
	Log_Level log_level;
	switch ( severity ) {
		case GL_DEBUG_SEVERITY_HIGH:          log_level = LogLevel_Error; break;

		case GL_DEBUG_SEVERITY_MEDIUM:
		case GL_DEBUG_SEVERITY_LOW:           log_level = LogLevel_Warning; break;

		case GL_DEBUG_SEVERITY_NOTIFICATION:
		default:                              log_level = LogLevel_Debug; break;
	}
	// The function itself is called only with the OpenGL debug output set up,
	// no need to wrap with `QLIGHT_DEBUG`.
	log_gl(
		log_level,
		StringViewFormat "::" StringViewFormat " (#%d, " StringViewFormat "): " StringViewFormat,
		StringViewArgument( source_name ),
		StringViewArgument( type_name ),
		id,
		StringViewArgument( severity_name ),
		StringViewArgument( msg )
	);
}

inline static u32
opengl_query_constant_as_u32( GLint constant, GLint *out_gl_result ) {
	glGetIntegerv( constant, out_gl_result );
	return ( u32 ) *out_gl_result;
}

static void
opengl_query_constants() {
	GL_Constants *gl = &g_renderer.gl_constants;
	GLint gl_result;

	gl->max_color_attachments       = opengl_query_constant_as_u32( GL_MAX_COLOR_ATTACHMENTS, &gl_result );
	gl->max_uniform_block_size      = opengl_query_constant_as_u32( GL_MAX_UNIFORM_BLOCK_SIZE, &gl_result );
	gl->max_uniform_locations       = opengl_query_constant_as_u32( GL_MAX_UNIFORM_LOCATIONS, &gl_result );
	gl->max_uniform_buffer_bindings = opengl_query_constant_as_u32( GL_MAX_UNIFORM_BUFFER_BINDINGS, &gl_result );
	gl->min_map_buffer_alignment    = opengl_query_constant_as_u32( GL_MIN_MAP_BUFFER_ALIGNMENT, &gl_result );
}

bool
renderer_init() {
	log_debug( "Initializing Renderer..." );
	if ( g_renderer.programs.data )
		return false;

//#if QLIGHT_DEBUG
#if 1
	glEnable( GL_DEBUG_OUTPUT );
	glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
	GLuint disabled_messages[] {
		/* Buffer detailed info */ 131185
	};
	glDebugMessageControl(
	    /*   source */ GL_DEBUG_SOURCE_API,
	    /*     type */ GL_DEBUG_TYPE_OTHER,
	    /* severity */ GL_DONT_CARE,
	    /*    count */ ARRAY_SIZE( disabled_messages ),
	    /*      ids */ disabled_messages,
	    /*  enabled */ GL_FALSE
	);
	glDebugMessageCallback( opengl_debug_message_callback, NULL );
#endif

	g_renderer.opengl_error_log = string_new( sys_allocator, RENDERER_OPENGL_ERROR_LOG_CAPACITY );
	g_renderer.opengl_info_log = string_new( sys_allocator, RENDERER_OPENGL_INFO_LOG_CAPACITY );

	g_renderer.frame_time.last = 0.0f;
	g_renderer.frame_time.current = 0.0f;
	g_renderer.frame_time.delta = 0.0f;

	renderer_set_clear_color( Vector4_f32( 0.0f )  );

	g_renderer.uniforms_transpose_matrix = false;
	g_renderer.framebuffers = array_new< Renderer_Framebuffer >( sys_allocator, RENDERER_INITIAL_FRAMEBUFFERS_CAPACITY );
	g_renderer.renderbuffers = array_new< Renderer_Renderbuffer >( sys_allocator, RENDERER_INITIAL_RENDERBUFFERS_CAPACITY );
	g_renderer.programs = array_new< Renderer_Shader_Program >( sys_allocator, RENDERER_INITIAL_PROGRAMS_CAPACITY );
	g_renderer.stages = array_new< Renderer_Shader_Stage >( sys_allocator, RENDERER_INITIAL_STAGES_CAPACITY );
	g_renderer.uniform_buffers = array_new< Renderer_Uniform_Buffer >( sys_allocator, RENDERER_INITIAL_UNIFORM_BUFFERS_CAPACITY );

	opengl_query_constants();

	g_renderer.device.vendor = string_view( ( const char * )glGetString( GL_VENDOR ) );
	g_renderer.device.name = string_view( ( const char * )glGetString( GL_RENDERER ) );

	renderer_create_default_framebuffer();
	constexpr Vector2_u16 TEMP_dimensions = { 1280, 720 };
	setup_geometry_buffer( TEMP_dimensions );
	setup_fullscreen_quad();

	g_renderer.output_channel = RendererOutputChannel_FinalColor;

	g_renderer.camera_position = NULL;
	g_renderer.view_matrix = NULL;
	g_renderer.projection_matrix = NULL;
	g_renderer.ambient_light = Vector3_f32( 0.1f, 0.1f, 0.1f );

	g_renderer.render_queue = array_new< Renderer_Render_Command >( sys_allocator, 32 );
	g_renderer.render_queue_material_sequence = array_new< u16 >( sys_allocator, 32 );

	create_default_textures();

	// Set facet winding order to clockwise.
	// Then, clockwise ordered facets are considered to be front-facing,
	//   while counter-clockwise ordered to be back-facing.
	glFrontFace( GL_CW );

	// Enable back-facing facets culling.
	glEnable( GL_CULL_FACE );

	log_debug( "Renderer has been initialized." );
	return true;
}

void
renderer_shutdown() {
	if ( !g_renderer.programs.data )
		return;

	array_free( &g_renderer.framebuffers );
	array_free( &g_renderer.renderbuffers );
	array_free( &g_renderer.programs );
	array_free( &g_renderer.stages );
	array_free( &g_renderer.uniform_buffers );

	array_free( &g_renderer.render_queue );
	array_free( &g_renderer.render_queue_material_sequence );
}

static void
opengl_preprocess_shader_stage( Renderer_Shader_Stage *stage ) {
	printf(
		"WARNING: opengl_preprocess_shader_stage( \"" StringViewFormat "\": Not implemented yet.\n",
		StringViewArgument( stage->name )
	);
}

Renderer_Shader_Stage *
renderer_find_shader_stage( StringView_ASCII name ) {
	ForIt( g_renderer.stages.data, g_renderer.stages.size ) {
		if ( string_equals( name, it.name ) )
			return &it;
	}}

	return NULL;
}

Renderer_Shader_Program *
renderer_find_shader_program( StringView_ASCII name ) {
	ForIt( g_renderer.programs.data, g_renderer.programs.size ) {
		if ( string_equals( name, it.name ) )
			return &it;
	}}

	return NULL;
}

bool
renderer_destroy_shader_stage( Renderer_Shader_Stage *stage ) {
	printf(
		"WARNING: renderer_destroy_shader_stage( \"" StringViewFormat "\" ): Not implemented yet.\n",
		StringViewArgument( stage->name )
	);
	Assert( false );
	return true;
}

bool
renderer_destroy_shader_program( Renderer_Shader_Program *program ) {
	printf(
		"WARNING: renderer_destroy_shader_program( \"" StringViewFormat "\" ): Not implemented yet.\n",
		StringViewArgument( program->name )
	);
	Assert( false );
	return true;
}

StringView_ASCII
renderer_shader_kind_name( Renderer_Shader_Kind kind ) {
	switch ( kind ) {
		case RendererShaderKind_Vertex:                 return "Vertex";
		case RendererShaderKind_TesselationControl:     return "Tesselation Control";
		case RendererShaderKind_TesselationEvaluation:  return "Tesselation Evaluation";
		case RendererShaderKind_Geometry:               return "Geometry";
		case RendererShaderKind_Fragment:               return "Fragment";
		case RendererShaderKind_Compute:                return "Compute";

		default: return string_view( ( const char * )NULL );
	}
}

#include <stdio.h> // @TODO: Remove

static GLuint
opengl_compile_shader_stage( Renderer_Shader_Stage *stage ) {
	GLuint shader_type = renderer_shader_kind_to_opengl( stage->kind );
	StringView_ASCII shader_kind = renderer_shader_kind_name( stage->kind );
	log_debug_gl( "Compiling '" StringViewFormat "' " StringViewFormat " stage shader...",
		StringViewArgument( stage->name ),
		StringViewArgument( shader_kind )
	);

	GLuint shader_id = glCreateShader( shader_type );
	const GLchar *sources[] = { stage->source_code.data };
	const GLint lengths[] = { ( GLint )stage->source_code.size };
	glShaderSource( shader_id, 1, sources, lengths );

	GLint compile_result;
	glCompileShader( shader_id );
	glGetShaderiv( shader_id, GL_COMPILE_STATUS, &compile_result );
	if ( compile_result != GL_TRUE ) {
		StringView_ASCII info_log = opengl_get_info_log( OpenGL_Shader, shader_id );

		log_error_gl( "Failed to compile '" StringViewFormat "' " StringViewFormat " stage shader! Error log:\n" StringViewFormat,
			StringViewArgument( stage->name ),
			StringViewArgument( shader_kind ),
			StringViewArgument( info_log )
		);

		// No need to delete it now, it will be deleted later
		//   in `renderer_create_and_compile_shader_program`
		//   whether it compiled successfully or not.
	} else {
		log_debug_gl( "Compiled '" StringViewFormat "' " StringViewFormat" shader stage.",
			StringViewArgument( stage->name ),
			StringViewArgument( shader_kind )
		);
	}

	return shader_id;
}

GLenum
renderer_shader_kind_to_opengl( Renderer_Shader_Kind kind ) {
	switch ( kind ) {
		case RendererShaderKind_Vertex: return GL_VERTEX_SHADER;
		case RendererShaderKind_TesselationControl: return GL_TESS_CONTROL_SHADER;
		case RendererShaderKind_TesselationEvaluation: return GL_TESS_EVALUATION_SHADER;
		case RendererShaderKind_Geometry: return GL_GEOMETRY_SHADER;
		case RendererShaderKind_Fragment: return GL_FRAGMENT_SHADER;
		case RendererShaderKind_Compute: return GL_COMPUTE_SHADER;

		default: return 0;
	};
}

Renderer_Shader_Stage *
renderer_load_shader_stage( StringView_ASCII name, Renderer_Shader_Kind kind, StringView_ASCII file_path ) {
	Renderer_Shader_Stage stage = {
		.name = name,
		.file_path = file_path,
		// .source_code
		.kind = kind
		// .opengl_shader
	};

	// Must be null-terminated!
	FILE *file = fopen( file_path.data, "r" );

	fseek( file, 0, SEEK_END );
	u32 file_size = ftell( file );
	rewind( file );

	stage.source_code = string_new( sys_allocator, file_size );
	stage.source_code.size = ( u32 )fread( stage.source_code.data, sizeof( char ), file_size, file );
	fclose( file );

	u32 stage_idx = array_add( &g_renderer.stages, stage );
	Renderer_Shader_Stage *stage_ptr = &g_renderer.stages.data[ stage_idx ];
	return stage_ptr;
}

Renderer_Shader_Kind_Bits
renderer_shader_kind_bit( Renderer_Shader_Kind kind ) {
	switch ( kind ) {
		case RendererShaderKind_Vertex:                 return RendererShaderKindBit_Vertex;
		case RendererShaderKind_TesselationControl:     return RendererShaderKindBit_TesselationControl;
		case RendererShaderKind_TesselationEvaluation:  return RendererShaderKindBit_TesselationEvaluation;
		case RendererShaderKind_Geometry:               return RendererShaderKindBit_Geometry;
		case RendererShaderKind_Fragment:               return RendererShaderKindBit_Fragment;
		case RendererShaderKind_Compute:                return RendererShaderKindBit_Compute;

		default: return 0;
	};
}

constexpr u32 SHADER_PROGRAM_VERTEX_ATTRIBUTES_INITIAL_CAPACITY = 6;
constexpr u32 SHADER_PROGRAM_UNIFORMS_INITIAL_CAPACITY = 8;
constexpr u32 SHADER_PROGRAM_UNIFORM_BUFFERS_INITIAL_CAPACITY = 1;

Renderer_Shader_Program *
renderer_create_and_compile_shader_program( StringView_ASCII name, ArrayView< Renderer_Shader_Stage * > shader_stages ) {
	log_debug( "Creating and compiling '" StringViewFormat "' shader program...",
		StringViewArgument( name )
	);
	Renderer_Shader_Program program = {
		.name = name,
		.vertex_attributes = array_new< Renderer_Vertex_Attribute >( sys_allocator, SHADER_PROGRAM_VERTEX_ATTRIBUTES_INITIAL_CAPACITY ),
		.uniforms = array_new< Renderer_Uniform >( sys_allocator, SHADER_PROGRAM_UNIFORMS_INITIAL_CAPACITY ),
		.uniform_buffers = array_new< Renderer_Uniform_Buffer >( sys_allocator, SHADER_PROGRAM_UNIFORM_BUFFERS_INITIAL_CAPACITY )
	};

	GL_CHECK( program.opengl_program = glCreateProgram() );
#ifdef QLIGHT_DEBUG
	glObjectLabel( GL_PROGRAM, program.opengl_program, name.size, name.data );
#endif

	// Iterate over passes shader stages.
	ForIt( shader_stages.data, shader_stages.size ) {
		Renderer_Shader_Kind_Bits kind_bit = renderer_shader_kind_bit( it->kind );
		if ( program.linked_shaders & kind_bit ) {
			// Complain and skip.
			StringView_ASCII shader_kind = renderer_shader_kind_name( it->kind );
			log_warning_gl(
				"WARNING: Trying to attach " StringFormat " stage shader to '" StringFormat "' shader program, but it already has one. Skipping.",
				StringArgumentValue( shader_kind ),
				StringArgumentValue( name )
			);
			continue;
		}

		it->opengl_shader = opengl_compile_shader_stage( it );
		glAttachShader( program.opengl_program, it->opengl_shader );
#ifdef QLIGHT_DEBUG
		glObjectLabel( GL_SHADER, it->opengl_shader, it->name.size, it->name.data );
#endif

		program.shaders[ it_index ] = it;
		program.linked_shaders |= kind_bit;
	}}

	GLint opengl_result;

	GL_CHECK( glLinkProgram( program.opengl_program ) );
	glGetProgramiv( program.opengl_program, GL_LINK_STATUS, &opengl_result );
	if ( opengl_result != GL_TRUE ) {
		StringView_ASCII info_log = opengl_get_info_log( OpenGL_Program, program.opengl_program );

		log_error_gl(
			"Failed to link '" StringFormat "' shader program! Error log:\n" StringFormat,
			StringArgumentValue( program.name ),
			StringViewArgument( info_log )
		);
	}

	GL_CHECK( glValidateProgram( program.opengl_program ) );
	glGetProgramiv( program.opengl_program, GL_VALIDATE_STATUS, &opengl_result );
	if ( opengl_result != GL_TRUE ) {
		StringView_ASCII info_log = opengl_get_info_log( OpenGL_Program, program.opengl_program );

		log_error_gl(
			"Failed to validate '" StringFormat "' shader program! Error log:\n" StringFormat,
			StringArgumentValue( program.name ),
			StringViewArgument( info_log )
		);
	}

	// Iterate over program's shaders.
	// No `.data` because it is a flat C array, not Array/ArrayView struct.
	ForIt( program.shaders, RendererShaderKind_COUNT ) {
		if ( !it )
			continue;

		Renderer_Shader_Kind_Bits kind_bit = renderer_shader_kind_bit( it->kind );
		if ( !( program.linked_shaders & kind_bit ) )
			continue;

		GL_CHECK( glDeleteShader( it->opengl_shader ) );
		glGetShaderiv( it->opengl_shader, GL_DELETE_STATUS, &opengl_result );
		if ( opengl_result != GL_TRUE ) {
			StringView_ASCII info_log = opengl_get_info_log( OpenGL_Program, program.opengl_program );

			log_error_gl(
				"Failed to mark '" StringFormat "' shader stage for deletion! Error log:\n" StringFormat,
				StringArgumentValue( it->name ),
				StringViewArgument( info_log )
			);
		}
	}}

	u32 program_idx = array_add( &g_renderer.programs, program );
	Renderer_Shader_Program * program_ptr = &g_renderer.programs.data[ program_idx ];
	log_debug( "Created and compiled '" StringViewFormat "' shader program.",
		StringViewArgument( name )
	);
	return program_ptr;
}

bool
renderer_shader_program_set_uniform( Renderer_Shader_Program *program, StringView_ASCII uniform_name, Renderer_Data_Type data_type, void *value ) {
	bool transpose = g_renderer.uniforms_transpose_matrix;

	// The function can determine whether the OpenGL call was successfull
	//   only if the OpenGL error logging is turned on.  Otherwise, it
	//   will always return true.
	// `success` can only be overwritten if `QLIGHT_OPENGL_ERROR_CHECKS` is defined
	//   and `glUniformXXX` call generates an error.
	bool success = true;
	bool *result = &success;
	ForIt( program->uniforms.data, program->uniforms.size ) {
		if ( string_equals( uniform_name, it.name ) ) {
			Assert( data_type == it.data_type );
			if ( data_type != it.data_type )
				return false;

			GLuint location = it.opengl_location;
			switch ( data_type ) {
				// Sampler2D ?
				case RendererDataType_s32:
					GL_CHECK_AND_STORE_RESULT( result, glUniform1i( location, *( s32 *)value ) ); break;
				case RendererDataType_f32:
					GL_CHECK_AND_STORE_RESULT( result, glUniform1f( location, *( f32 *)value ) ); break;
				case RendererDataType_Vector2_f32:
					GL_CHECK_AND_STORE_RESULT( result, glUniform2fv( location, 1, ( f32 * )value ) ); break;
				case RendererDataType_Vector3_f32:
					GL_CHECK_AND_STORE_RESULT( result, glUniform3fv( location, 1, ( f32 * )value ) ); break;
				case RendererDataType_Vector4_f32:
					GL_CHECK_AND_STORE_RESULT( result, glUniform4fv( location, 1, ( f32 * )value ) ); break;
				case RendererDataType_Matrix3x3_f32:
					GL_CHECK_AND_STORE_RESULT( result, glUniformMatrix3fv( location, 1, transpose, ( f32 * )value ) ); break;
				case RendererDataType_Matrix4x4_f32:
					GL_CHECK_AND_STORE_RESULT( result, glUniformMatrix4fv( location, 1, transpose, ( f32 * )value ) ); break;
				default:
					AssertMessage( false, "Unsupported type" );
					return false;
			}
		}
	}}

	return success;
}

u32
renderer_shader_program_update_uniform_locations( Renderer_Shader_Program *program ) {
	u32 updated = 0;
	ForIt( program->uniforms.data, program->uniforms.size ) {
		// @Warning: uniform's name must be null-terminated!
		// @TODO: Check for OpenGL errors
		it.opengl_location = glGetUniformLocation( program->opengl_program, it.name.data );
		if ( it.opengl_location != -1 )
			updated += 1;
		else
			log_error( "Failed to update Uniform '" StringViewFormat "' location of '" StringViewFormat "' shader program!",
				StringViewArgument( it.name ),
				StringViewArgument( program->name )
			);
	}}

	log_debug( "Updated %u/%u uniform locations of '" StringViewFormat "' shader program.",
		updated,
		program->uniforms.size,
		StringViewArgument( program->name )
	);
	return updated;
}

void
renderer_set_uniforms_transpose_matrix( bool value ) {
	g_renderer.uniforms_transpose_matrix = value;
}

u32
renderer_data_type_size( Renderer_Data_Type data_type ) {
	switch ( data_type ) {
		case RendererDataType_s8:
		case RendererDataType_u8:
			return 1;

		case RendererDataType_s16:
		case RendererDataType_u16:
		case RendererDataType_f16:
			return 2;

		case RendererDataType_s32:
		case RendererDataType_u32:
		case RendererDataType_f32:
			return 4;

		case RendererDataType_f64:
			return 8;

		case RendererDataType_Vector2_f32:    return sizeof( f32 ) * 2;
		case RendererDataType_Vector3_f32:    return sizeof( f32 ) * 3;
		case RendererDataType_Vector4_f32:    return sizeof( f32 ) * 4;

		case RendererDataType_Matrix2x2_f32:  return sizeof( f32 ) * 2 * 2;
		case RendererDataType_Matrix2x3_f32:  return sizeof( f32 ) * 2 * 3;
		case RendererDataType_Matrix2x4_f32:  return sizeof( f32 ) * 2 * 4;

		case RendererDataType_Matrix3x2_f32:  return sizeof( f32 ) * 3 * 2;
		case RendererDataType_Matrix3x3_f32:  return sizeof( f32 ) * 3 * 3;
		case RendererDataType_Matrix3x4_f32:  return sizeof( f32 ) * 3 * 4;

		case RendererDataType_Matrix4x2_f32:  return sizeof( f32 ) * 4 * 2;
		case RendererDataType_Matrix4x3_f32:  return sizeof( f32 ) * 4 * 3;
		case RendererDataType_Matrix4x4_f32:  return sizeof( f32 ) * 4 * 4;

		default:
			return 0;
	};
}

GLenum
renderer_data_type_to_opengl_type( Renderer_Data_Type data_type ) {
	switch ( data_type ) {
		case RendererDataType_s8:   return GL_BYTE;
		case RendererDataType_u8:   return GL_UNSIGNED_BYTE;

		case RendererDataType_s16:  return GL_SHORT;
		case RendererDataType_u16:  return GL_UNSIGNED_SHORT;
		case RendererDataType_f16:  return GL_HALF_FLOAT;

		case RendererDataType_s32:  return GL_INT;
		case RendererDataType_u32:  return GL_UNSIGNED_INT;
		case RendererDataType_f32:  return GL_FLOAT;

		case RendererDataType_f64:  return GL_DOUBLE;

		// GL_FIXED
		// GL_INT_2_10_10_10_REV
		// GL_UNSIGNED_INT_2_10_10_10_REV
		// GL_UNSIGNED_INT_10F_11F_11F_REV

		default:                    return GL_INVALID_VALUE;
	}
}

Renderer_Renderbuffer_ID
renderer_create_renderbuffer(
	StringView_ASCII name,
	Vector2_u16 dimensions,
	Renderer_Framebuffer_Attachment_Point attachment_point,
	GLenum opengl_storage_format
) {
	Renderer_Renderbuffer renderbuffer = {
		.name = name,
		.attachment_point = attachment_point,
		.opengl_storage_format = opengl_storage_format
		// .opengl_renderbuffer
	};

	// Legacy: glGenRenderbuffers (cannot be used in Direct State Access functions (glNamedRenderbuffer).
	GL_CHECK( glCreateRenderbuffers( 1, &renderbuffer.opengl_renderbuffer ) );
#ifdef QLIGHT_DEBUG
	glObjectLabel( GL_RENDERBUFFER, renderbuffer.opengl_renderbuffer, name.size, name.data );
#endif
	// glBindRenderbuffer( GL_RENDERBUFFER, renderbuffer.opengl_renderbuffer );
	// glRenderbufferStorage( GL_RENDERBUFFER, ... );
	glNamedRenderbufferStorage(
		/*   renderbuffer */ renderbuffer.opengl_renderbuffer,
		/* internalformat */ opengl_storage_format,
		/*          width */ ( GLsizei )dimensions.width,
		/*         height */ ( GLsizei )dimensions.height
	);

	u32 renderbuffer_idx = array_add( &g_renderer.renderbuffers, renderbuffer );
	StringView_ASCII attachment_point_name = renderer_framebuffer_attachment_point_name( renderbuffer.attachment_point );
	StringView_ASCII format_name = opengl_storage_format_name( opengl_storage_format );
	log_info( "Created Renderbuffer '" StringViewFormat "' (#%u, %hux%hu, " StringViewFormat ", " StringViewFormat ").",
		StringViewArgument( renderbuffer.name ),
		renderbuffer_idx,
		dimensions.width,
		dimensions.height,
		StringViewArgument( attachment_point_name ),
		StringViewArgument( format_name )
	);
	return renderbuffer_idx;
}

Renderer_Renderbuffer_ID
renderer_find_renderbuffer( StringView_ASCII name ) {
	ForIt( g_renderer.renderbuffers.data, g_renderer.renderbuffers.size ) {
		if ( string_equals( name, it.name ) )
			return it_index;
	}}

	return INVALID_RENDERBUFFER_ID;
}

Renderer_Renderbuffer *
renderer_renderbuffer_instance( Renderer_Renderbuffer_ID renderbuffer_id ) {
	if ( renderbuffer_id >= g_renderer.renderbuffers.size )
		return NULL;

	return &g_renderer.renderbuffers.data[ renderbuffer_id ];
}

Renderer_Framebuffer_ID
renderer_create_framebuffer( StringView_ASCII name ) {
	Renderer_Framebuffer framebuffer = {
		.name = name,
		.attachments = array_new< Renderer_Framebuffer_Attachment >( sys_allocator, RENDERER_INITIAL_FRAMEBUFFER_ATTACHMENTS_CAPACITY )
		// .opengl_framebuffer
	};

	// Legacy: glGenFramebuffers (cannot be used in Direct State Access functions (glNamedFramebuffer).
	GL_CHECK( glCreateFramebuffers( 1, &framebuffer.opengl_framebuffer ) );
#ifdef QLIGHT_DEBUG
	glObjectLabel( GL_FRAMEBUFFER, framebuffer.opengl_framebuffer, name.size, name.data );
#endif

	u32 framebuffer_idx = array_add( &g_renderer.framebuffers, framebuffer );
	log_info( "Created Framebuffer '" StringViewFormat "' (#%u).",
		StringViewArgument( framebuffer.name ),
		framebuffer_idx
	);
	return framebuffer_idx;
}

Renderer_Framebuffer_ID
renderer_find_framebuffer( StringView_ASCII name ) {
	ForIt( g_renderer.framebuffers.data, g_renderer.framebuffers.size ) {
		if ( string_equals( name, it.name ) )
			return it_index;
	}}

	return INVALID_FRAMEBUFFER_ID;
}

Renderer_Framebuffer *
renderer_framebuffer_instance( Renderer_Framebuffer_ID framebuffer_id ) {
	if ( framebuffer_id >= g_renderer.framebuffers.size )
		return NULL;

	return &g_renderer.framebuffers.data[ framebuffer_id ];
}

bool
renderer_renderbuffer_attach_to_framebuffer( Renderer_Framebuffer_ID framebuffer_id, Renderer_Renderbuffer_ID renderbuffer_id, Renderer_Framebuffer_Attachment_Point attachment_point ) {
	Renderer_Framebuffer *framebuffer = renderer_framebuffer_instance( framebuffer_id );
	Renderer_Renderbuffer *renderbuffer = renderer_renderbuffer_instance( renderbuffer_id );

	GLenum opengl_framebuffer_attachment = renderer_framebuffer_attachment_point_to_opengl( attachment_point );
	Assert( opengl_framebuffer_attachment != GL_INVALID_ENUM );
	if ( opengl_framebuffer_attachment == GL_INVALID_ENUM )
		return false;

	// glFramebufferRenderbuffer( GL_FRAMEBUFFER, ... );
	glNamedFramebufferRenderbuffer(
		/*        framebuffer */ framebuffer->opengl_framebuffer,
		/*         attachment */ opengl_framebuffer_attachment,
		/* renderbuffertarget */ GL_RENDERBUFFER,
		/*       renderbuffer */ renderbuffer->opengl_renderbuffer
	);

	Renderer_Framebuffer_Attachment attachment = {
		.attachment_point = attachment_point,
		.bits = RendererFramebufferAttachmentBit_IsRenderbuffer,
	};
	attachment._renderbuffer_id = renderbuffer_id;
	// If it is a Depth or Stencil or DepthStencil attachment, mark it as active.
	// (There is no way to change it in OpenGL, so it always stays active, I guess?)
	if ( attachment_point == RendererFramebufferAttachmentPoint_Depth ||
		attachment_point == RendererFramebufferAttachmentPoint_Stencil ||
		attachment_point == RendererFramebufferAttachmentPoint_DepthStencil
	) {
		attachment.bits |= RendererFramebufferAttachmentBit_IsActive;
	}
	array_add( &framebuffer->attachments, attachment );

	StringView_ASCII attachment_point_name = renderer_framebuffer_attachment_point_name( renderbuffer->attachment_point );
	log_debug(
		"Attached Renderbuffer '" StringViewFormat "' (#%u) to Framebuffer '" StringViewFormat "' (#%u) at " StringViewFormat ".",
		StringViewArgument( renderbuffer->name ),
		renderbuffer_id,
		StringViewArgument( framebuffer->name ),
		framebuffer_id,
		StringViewArgument( attachment_point_name )
	);
	return true;
}

GLenum
renderer_framebuffer_attachment_point_to_opengl( Renderer_Framebuffer_Attachment_Point attachment_point ) {
	switch ( attachment_point ) {
		case RendererFramebufferAttachmentPoint_Color0:        return GL_COLOR_ATTACHMENT0;
		case RendererFramebufferAttachmentPoint_Color1:        return GL_COLOR_ATTACHMENT1;
		case RendererFramebufferAttachmentPoint_Color2:        return GL_COLOR_ATTACHMENT2;
		case RendererFramebufferAttachmentPoint_Color3:        return GL_COLOR_ATTACHMENT3;
		case RendererFramebufferAttachmentPoint_Color4:        return GL_COLOR_ATTACHMENT4;
		case RendererFramebufferAttachmentPoint_Color5:        return GL_COLOR_ATTACHMENT5;
		case RendererFramebufferAttachmentPoint_Color6:        return GL_COLOR_ATTACHMENT6;
		case RendererFramebufferAttachmentPoint_Color7:        return GL_COLOR_ATTACHMENT7;
		case RendererFramebufferAttachmentPoint_Color8:        return GL_COLOR_ATTACHMENT8;
		case RendererFramebufferAttachmentPoint_Color9:        return GL_COLOR_ATTACHMENT9;
		case RendererFramebufferAttachmentPoint_Color10:       return GL_COLOR_ATTACHMENT10;
		case RendererFramebufferAttachmentPoint_Color11:       return GL_COLOR_ATTACHMENT11;
		case RendererFramebufferAttachmentPoint_Color12:       return GL_COLOR_ATTACHMENT12;
		case RendererFramebufferAttachmentPoint_Color13:       return GL_COLOR_ATTACHMENT13;
		case RendererFramebufferAttachmentPoint_Color14:       return GL_COLOR_ATTACHMENT14;
		case RendererFramebufferAttachmentPoint_Color15:       return GL_COLOR_ATTACHMENT15;

		case RendererFramebufferAttachmentPoint_Depth:         return GL_DEPTH_ATTACHMENT;
		case RendererFramebufferAttachmentPoint_Stencil:       return GL_STENCIL_ATTACHMENT;
		case RendererFramebufferAttachmentPoint_DepthStencil:  return GL_DEPTH_STENCIL_ATTACHMENT;


		case RendererFramebufferAttachmentPoint_None:
		default:                                          return GL_INVALID_ENUM;
	}
}

bool
renderer_is_framebuffer_complete( Renderer_Framebuffer_ID framebuffer_id ) {
	Renderer_Framebuffer *framebuffer = renderer_framebuffer_instance( framebuffer_id );
	GLenum opengl_framebuffer_status = glCheckNamedFramebufferStatus( framebuffer->opengl_framebuffer, GL_FRAMEBUFFER );
	bool complete = ( opengl_framebuffer_status == GL_FRAMEBUFFER_COMPLETE );
	return complete;
}

void
renderer_set_active_framebuffer_color_attachment_points( Renderer_Framebuffer_ID framebuffer_id, ArrayView< Renderer_Framebuffer_Attachment_Point > color_attachment_points ) {
	if ( color_attachment_points.size > g_renderer.gl_constants.max_color_attachments ) {
		Assert( color_attachment_points.size > g_renderer.gl_constants.max_color_attachments );
		return;
	}

	Renderer_Framebuffer *framebuffer = renderer_framebuffer_instance( framebuffer_id );

	// Can we do it nicely without memory allocation?
	Array< GLenum > opengl_color_attachments = array_new< GLenum >( sys_allocator, color_attachment_points.size );
	// GLenum opengl_color_attachments[ GL_MAX_COLOR_ATTACHMENTS ];

	// Mark all color attachments as not active (depth/stencil is always active).
	ForIt( framebuffer->attachments.data, framebuffer->attachments.size ) {
		if ( renderer_framebuffer_color_attachment_point_is_valid( it.attachment_point ) ) {
			// Color attachment, set as not active.
			it.bits &= ~RendererFramebufferAttachmentBit_IsActive;
		}
	}}

	ForIt( color_attachment_points.data, color_attachment_points.size ) {
		// Mark attachment with this color attachment point as active.
		ForIt2( framebuffer->attachments.data, framebuffer->attachments.size ) {
			if ( it2.attachment_point == it ) {
				it2.bits |= RendererFramebufferAttachmentBit_IsActive;
				break;
			}
		}}

		// Check whether there are non-color attachments.
		Assert( renderer_framebuffer_color_attachment_point_is_valid( it ) );

		GLenum opengl_color_attachment = renderer_framebuffer_attachment_point_to_opengl( it );
		// opengl_color_attachments[ attachment_idx ] = opengl_color_attachment;
		array_add( &opengl_color_attachments, opengl_color_attachment );
	}}

	glNamedFramebufferDrawBuffers( framebuffer->opengl_framebuffer, opengl_color_attachments.size, opengl_color_attachments.data );
	array_free( &opengl_color_attachments );
}

static void
geometry_pass_use_material( Material *material ) {
	// We do not bind material's shader here since it is a Geometry pass
	Renderer_Shader_Program *gbuffer_shader = g_renderer.gbuffer.shader_program;

	// These are pointers to bound camera's matrices.
	renderer_shader_program_set_uniform( gbuffer_shader, "view", RendererDataType_Matrix4x4_f32, g_renderer.view_matrix );
	renderer_shader_program_set_uniform( gbuffer_shader, "projection", RendererDataType_Matrix4x4_f32, g_renderer.projection_matrix );

	/* Diffuse texture */

	// uniform sampler2D texture_diffuse0;
	s32 texture_diffuse_index = 0; // make configurable
	renderer_shader_program_set_uniform(
		gbuffer_shader,
		"texture_diffuse0",
		RendererDataType_s32,
		&texture_diffuse_index
	);
	Texture_ID texture_diffuse_id = ( material->diffuse != INVALID_TEXTURE_ID ) ? material->diffuse : g_renderer.texture_purple_checkers;
	renderer_bind_texture( texture_diffuse_index, texture_diffuse_id );

	/* Normal map texture */

	// uniform sampler2D texture_normal0;
	s32 texture_normal_index = 1; // make configurable
	renderer_shader_program_set_uniform(
		gbuffer_shader,
		"texture_normal0",
		RendererDataType_s32,
		&texture_normal_index
	);
	Texture_ID texture_normal_id = ( material->normal_map != INVALID_TEXTURE_ID ) ? material->normal_map : g_renderer.texture_white;
	renderer_bind_texture( texture_normal_index, texture_normal_id );

	/* Specular map texture */

	// uniform sampler2D texture_specular0;
	s32 texture_specular_index = 2; // make configurable
	renderer_shader_program_set_uniform(
		gbuffer_shader,
		"texture_specular0",
		RendererDataType_s32,
		&texture_specular_index
	);
	Texture_ID texture_specular_id = ( material->specular_map != INVALID_TEXTURE_ID ) ? material->specular_map : g_renderer.texture_white;
	renderer_bind_texture( texture_specular_index, texture_specular_id );
}

static void
geometry_pass_draw_same_material_commands( ArrayView< Renderer_Render_Command > commands ) {
	Material_ID material_id = commands.data[ 0 ].material_id;
	Material *material = material_instance( material_id );
	Renderer_Shader_Program *gbuffer_shader = g_renderer.gbuffer.shader_program;
	renderer_bind_shader_program( gbuffer_shader );
	geometry_pass_use_material( material );

	ForIt( commands.data, commands.size ) {
		Mesh *mesh = mesh_instance( it.mesh_id );
		transform_recalculate_dirty_matrices( it.transform );
		renderer_shader_program_set_uniform( gbuffer_shader, "model", RendererDataType_Matrix4x4_f32, &it.transform->model_matrix );
		renderer_shader_program_set_uniform( gbuffer_shader, "normal_matrix", RendererDataType_Matrix3x3_f32, &it.transform->normal_matrix );

		glBindVertexArray( mesh->opengl_vao );
		GLenum index_type = index_type_size_to_opengl( mesh->indices.item_size );
		glDrawElements(
			/*    mode */ GL_TRIANGLES,
			/*   count */ mesh->indices.size,
			/*    type */ index_type,
			/* indices */ NULL
		);
	}}
}

// TODO: Remove
#include "opengl.h"
extern Screen screen;

static void
draw_pass_geometry() {
	glEnable( GL_DEPTH_TEST );

	renderer_bind_framebuffer( g_renderer.gbuffer.framebuffer );
	Renderer_Framebuffer *geometry_framebuffer = renderer_framebuffer_instance( g_renderer.gbuffer.framebuffer );
	Vector2_u16 dimensions = g_renderer.gbuffer.dimensions;
	glViewport( 0, 0, ( GLsizei )dimensions.width, ( GLsizei )dimensions.height );

	// Clear Geometry framebuffer Position attachment texture
	glClearNamedFramebufferfv(
		/* framebuffer */ geometry_framebuffer->opengl_framebuffer,
		/*      buffer */ GL_COLOR,
		/*  drawbuffer */ 0,  // Attachment  index
		/*       value */ &g_renderer.clear_color.x
	);

	// Clear Geometry framebuffer Normal Map attachment texture
	glClearNamedFramebufferfv(
		/* framebuffer */ geometry_framebuffer->opengl_framebuffer,
		/*      buffer */ GL_COLOR,
		/*  drawbuffer */ 1,  // Attachment  index
		/*       value */ &g_renderer.clear_color.x
	);

	// Clear Geometry framebuffer Color/Specular attachment texture
	glClearNamedFramebufferfv(
		/* framebuffer */ geometry_framebuffer->opengl_framebuffer,
		/*      buffer */ GL_COLOR,
		/*  drawbuffer */ 2,  // Attachment  index
		/*       value */ &g_renderer.clear_color.x
	);

	// Clear Geometry framebuffer's deapth-stencil attachment renderbuffer
	glClearNamedFramebufferfi(
		/* framebuffer */ geometry_framebuffer->opengl_framebuffer,
		/*      buffer */ GL_DEPTH_STENCIL,
		/*  drawbuffer */ 0,  // Must be 0 in this case.
		/*       depth */ 1.0f,  // In the range of [0, 1]: 0.0 - near plane; 1.0 - far plane. So, default is the farthest one.
		/*     stencil */ 0  // Neutral value
	);

	u32 command_idx = 0;
	ForIt( g_renderer.render_queue_material_sequence.data, g_renderer.render_queue_material_sequence.size ) {
		Renderer_Render_Command *command_ptr = &g_renderer.render_queue.data[ command_idx ];
		ArrayView< Renderer_Render_Command > commands = { .size = it, .data = command_ptr };
		geometry_pass_draw_same_material_commands( commands );
		command_idx += it;
	}}

	Assert( command_idx == g_renderer.render_queue.size );
}

static void
lighting_pass_use_material( Material *material ) {
	renderer_shader_program_set_uniform(
		material->shader_program,
		"view_position",
		RendererDataType_Vector3_f32,
		g_renderer.camera_position
	);

	renderer_shader_program_set_uniform(
		material->shader_program,
		"ambient",
		RendererDataType_Vector3_f32,
		&g_renderer.ambient_light
	);

	renderer_shader_program_set_uniform(
		material->shader_program,
		"shininess_exponent",
		RendererDataType_f32,
		&material->shininess_exponent
	);

	// Uniform_Buffer_Lights gets updated in `maps_lights_manager_update()`
	//   every time light entity added/removed.
}

static void lighting_pass_use_gbuffer_textures( Renderer_Shader_Program *material_shader ) {

	/* G-Buffer Position texture */

	s32 texture_position_index = 0; // make configurable
	renderer_shader_program_set_uniform(
		material_shader,
		"gbuffer_position",
		RendererDataType_s32,
		&texture_position_index
	);
	Texture_ID gbuffer_texture_position = g_renderer.gbuffer.texture_position;
	Texture_ID texture_position_id = ( gbuffer_texture_position != INVALID_TEXTURE_ID ) ? gbuffer_texture_position : g_renderer.texture_black;
	renderer_bind_texture( texture_position_index, texture_position_id );

	/* G-Buffer Normal texture */

	s32 texture_normal_index = 1; // make configurable
	renderer_shader_program_set_uniform(
		material_shader,
		"gbuffer_normal",
		RendererDataType_s32,
		&texture_normal_index
	);
	Texture_ID gbuffer_texture_normal = g_renderer.gbuffer.texture_normal;
	Texture_ID texture_normal_id = ( gbuffer_texture_normal != INVALID_TEXTURE_ID ) ? gbuffer_texture_normal : g_renderer.texture_black;
	renderer_bind_texture( texture_normal_index, texture_normal_id );

	/* G-Buffer Diffuse/Specular texture */

	s32 texture_diffuse_specular_index = 2; // make configurable
	renderer_shader_program_set_uniform(
		material_shader,
		"gbuffer_diffuse_specular",
		RendererDataType_s32,
		&texture_diffuse_specular_index
	);
	Texture_ID gbuffer_texture_diffuse_specular = g_renderer.gbuffer.texture_color_specular;
	Texture_ID texture_diffuse_specular_id = ( gbuffer_texture_diffuse_specular != INVALID_TEXTURE_ID ) ? gbuffer_texture_diffuse_specular : g_renderer.texture_black;
	renderer_bind_texture( texture_diffuse_specular_index, texture_diffuse_specular_id );
}

static void
lighting_pass_draw_same_material_commands( ArrayView< Renderer_Render_Command > commands ) {
	Material_ID material_id = commands.data[ 0 ].material_id;
	Material *material = material_instance( material_id );
	Renderer_Shader_Program *material_shader = material->shader_program;
	renderer_bind_shader_program( material->shader_program );
	lighting_pass_use_gbuffer_textures( material->shader_program );
	lighting_pass_use_material( material );

	draw_fullscreen_quad();
}

static void
draw_pass_lighting() {
	glDisable( GL_DEPTH_TEST );

	// Fullscreen Quad is in clockwise winding order, it will not be culled.
	// glDisable( GL_CULL_FACE );

	renderer_bind_framebuffer( 0 ); // Default framebuffer
	Renderer_Framebuffer *default_framebuffer = renderer_framebuffer_instance( 0 );
	Vector2_u16 dimensions = { ( u16 )screen.width, ( u16 )screen.height };
	glViewport( 0, 0, ( GLsizei )dimensions.width, ( GLsizei )dimensions.height );

	// Clear Backbuffer framebuffer color attachment texture
	glClearNamedFramebufferfv(
		/* framebuffer */ default_framebuffer->opengl_framebuffer,
		/*      buffer */ GL_COLOR,
		/*  drawbuffer */ 0,  // Attachment  index
		/*       value */ &g_renderer.clear_color.x
	);

	u32 command_idx = 0;
	ForIt( g_renderer.render_queue_material_sequence.data, g_renderer.render_queue_material_sequence.size ) {
		Renderer_Render_Command *command_ptr = &g_renderer.render_queue.data[ command_idx ];
		ArrayView< Renderer_Render_Command > commands = { .size = it, .data = command_ptr };
		lighting_pass_draw_same_material_commands( commands );
		command_idx += it;
	}}

	Assert( command_idx == g_renderer.render_queue.size );

}

static void
post_processing_pass_draw() {

}

static void
ui_pass_draw() {

}

static void
sort_render_queue() {
	Array< Renderer_Render_Command > *queue = &g_renderer.render_queue;
	Renderer_Render_Command *cmd_a = NULL;
	Renderer_Render_Command *cmd_b;
	Renderer_Render_Command temp;
	u16 material_sequence = 1;
	array_clear( &g_renderer.render_queue_material_sequence );
	for ( u32 a = 0; a < queue->size; a += 1 ) {
		cmd_a = &queue->data[ a ];
		cmd_b = NULL;
		for ( u32 b = a + 1; b < queue->size; b += 1 ) {
			cmd_b = &queue->data[ b ];
			if ( cmd_b->material_id < cmd_a->material_id ) {
				temp = *cmd_a;
				*cmd_a = *cmd_b;
				*cmd_b = temp;
			}
		}

		if ( cmd_b ) {
			if ( cmd_a->material_id == cmd_b->material_id ) {
				material_sequence += 1;
			} else {
				array_add( &g_renderer.render_queue_material_sequence, material_sequence );
				material_sequence = 1;
			}
		} else {
			array_add( &g_renderer.render_queue_material_sequence, material_sequence );
		}
	}
}

void
renderer_draw_frame() {
	sort_render_queue();
	draw_pass_geometry();
	draw_pass_lighting();
	// renderer_draw_post_processsing_pass();
	// renderer_draw_ui_pass();

	g_renderer.frame_time.current = ( f32 )glfwGetTime();
	g_renderer.frame_time.delta = ( g_renderer.frame_time.current - g_renderer.frame_time.last ) * 1000.0f; // sec -> ms
	g_renderer.frame_time.last = g_renderer.frame_time.current;

	Renderer_Framebuffer_Attachment_Point attachment_point = RendererFramebufferAttachmentPoint_None;
	switch ( g_renderer.output_channel ) {
		case RendererOutputChannel_Position:
			attachment_point = RendererFramebufferAttachmentPoint_Color0;
			break;
		case RendererOutputChannel_Normal:
			attachment_point = RendererFramebufferAttachmentPoint_Color1;
			break;
		case RendererOutputChannel_DiffuseSpecular:
			attachment_point = RendererFramebufferAttachmentPoint_Color2;
			break;

		case RendererOutputChannel_FinalColor:
		default:
			break;
	}

	if ( attachment_point != RendererFramebufferAttachmentPoint_None ) {
		Renderer_Framebuffer *geometry_framebuffer = renderer_framebuffer_instance( g_renderer.gbuffer.framebuffer );
		GLenum opengl_attachment = renderer_framebuffer_attachment_point_to_opengl( attachment_point );
		glNamedFramebufferReadBuffer( geometry_framebuffer->opengl_framebuffer, opengl_attachment );
		glBlitNamedFramebuffer(
			/*           source */ geometry_framebuffer->opengl_framebuffer,
			/*      destination */ 0,
			/*      source rect */ 0, 0, 1280, 720,
			/* destination rect */ 0, 0, screen.width, screen.height,
			/*             mask */ GL_COLOR_BUFFER_BIT,
			/*           filter */ GL_LINEAR
		);
	}

	array_clear( &g_renderer.render_queue );
}

bool
renderer_bind_framebuffer( Renderer_Framebuffer_ID framebuffer_id ) {
	Assert( framebuffer_id <= g_renderer.framebuffers.size );
	if ( framebuffer_id > g_renderer.framebuffers.size ) {
		// report error
		return false;
	}

	Renderer_Framebuffer *framebuffer = renderer_framebuffer_instance( framebuffer_id );
	glBindFramebuffer( GL_FRAMEBUFFER, framebuffer->opengl_framebuffer );

	return true;
}

bool
renderer_bind_shader_program( Renderer_Shader_Program *program ) {
	Assert( program );
	if ( !program )
		return false;

	Assert( program->opengl_program != 0 );
	glUseProgram( program->opengl_program );
	return true;
}

bool
renderer_bind_texture( u32 texture_slot_idx, Texture_ID texture_id ) {
	// if  ( texture_slot_idx > MAX_TEXTURES )
	// return false;

	Texture *texture = texture_instance( texture_id );
	Assert( texture );
	if ( !texture )
		return false;

	Assert( texture->opengl_id != 0 );
	if ( texture->opengl_id == 0 )
		return false;

	glActiveTexture( GL_TEXTURE0 + texture_slot_idx );
	glBindTexture( GL_TEXTURE_2D, texture->opengl_id );
	// glActiveTexture( GL_TEXTURE0 );
	return true;
}

void
renderer_queue_draw_command( Mesh_ID mesh_id, Material_ID material_id, Transform *transform ) {
	array_add( &g_renderer.render_queue, Renderer_Render_Command {
		.mesh_id = mesh_id,
		.material_id = material_id,
		.transform = transform
	} );
}

void
renderer_set_view_matrix_pointer( Matrix4x4_f32 *view ) {
	Assert( view );
	g_renderer.view_matrix = view;
}

void
renderer_set_projection_matrix_pointer( Matrix4x4_f32 *projection ) {
	Assert( projection );
	g_renderer.projection_matrix = projection;
}

Renderer_Framebuffer_Attachment_Point
renderer_texture_channels_to_framebuffer_attachment_point( Texture_Channels channels ) {
	switch ( channels ) {
		case TextureChannels_Red:
		case TextureChannels_RG:
		case TextureChannels_RGB:
		case TextureChannels_RGBA:          return RendererFramebufferAttachmentPoint_Color0;

		case TextureChannels_Depth:         return RendererFramebufferAttachmentPoint_Depth;
		case TextureChannels_Stencil:       return RendererFramebufferAttachmentPoint_Stencil;
		case TextureChannels_DepthStencil:  return RendererFramebufferAttachmentPoint_DepthStencil;

		case TextureChannels_None:
		default:                          return RendererFramebufferAttachmentPoint_None;
	}
}

bool
renderer_framebuffer_color_attachment_point_is_valid( Renderer_Framebuffer_Attachment_Point color_attachment_point ) {
	bool is_valid = (
		color_attachment_point >= RendererFramebufferAttachmentPoint_Color0 &&
		color_attachment_point <= g_renderer.gl_constants.max_color_attachments - 1
	);
	return is_valid;
}

bool
renderer_texture_attach_to_framebuffer( Texture_ID texture_id, Renderer_Framebuffer_ID framebuffer_id, Renderer_Framebuffer_Attachment_Point attachment_point ) {
	Texture *texture = texture_instance( texture_id );
	Renderer_Framebuffer *framebuffer = renderer_framebuffer_instance( framebuffer_id );
	Renderer_Framebuffer_Attachment_Point expected_attachment_point = renderer_texture_channels_to_framebuffer_attachment_point( texture->channels );
	if ( expected_attachment_point == RendererFramebufferAttachmentPoint_Color0 ) {
		// This is a Color attachment.
		bool is_valid_color_attachment = renderer_framebuffer_color_attachment_point_is_valid( attachment_point );
		Assert( is_valid_color_attachment );
		if ( !is_valid_color_attachment )
			return false;
	} else {
		// This is a Depth / Stencil / DepthStencil attachment.
		// Those are mapped one-to-one.
		Assert( attachment_point == expected_attachment_point );
		if ( attachment_point != expected_attachment_point )
			return false;
	}

	GLenum opengl_framebuffer_attachment = renderer_framebuffer_attachment_point_to_opengl( attachment_point );
	glNamedFramebufferTexture(
		/* framebuffer */ framebuffer->opengl_framebuffer,
		/*  attachment */ opengl_framebuffer_attachment,
		/*     texture */ texture->opengl_id,
		/*       level */ 0  // mipmap
	);

	Renderer_Framebuffer_Attachment attachment = {
		.attachment_point = attachment_point,
		.bits = 0
	};
	// If it is a Depth or Stencil or DepthStencil attachment, mark it as active.
	// (There is no way to change it in OpenGL, so it always stays active, I guess?)
	if ( attachment_point == RendererFramebufferAttachmentPoint_Depth ||
		attachment_point == RendererFramebufferAttachmentPoint_Stencil ||
		attachment_point == RendererFramebufferAttachmentPoint_DepthStencil
	) {
		attachment.bits |= RendererFramebufferAttachmentBit_IsActive;
	}
	attachment._texture_id = texture_id;
	array_add( &framebuffer->attachments, attachment );

	StringView_ASCII attachment_point_name = renderer_framebuffer_attachment_point_name( attachment_point );
	log_debug(
		"Attached Texture '" StringViewFormat "' (#%u) to Framebuffer '" StringViewFormat "' (#%u) at " StringViewFormat ".",
		StringViewArgument( texture->name ),
		texture_id,
		StringViewArgument( framebuffer->name ),
		framebuffer_id,
		StringViewArgument( attachment_point_name )
	);
	return true;
}

static void
opengl_create_and_bind_texture_2d( GLuint *id, StringView_ASCII debug_name ) {
	glGenTextures( 1, id );
	glBindTexture( GL_TEXTURE_2D, *id );
#ifdef QLIGHT_DEBUG
	if ( debug_name.size > 0 )
		glObjectLabel( GL_TEXTURE, *id, debug_name.size, debug_name.data );
	log_debug_gl( "Created and bound 2D Texture '" StringViewFormat "' (gl_id: %u).",
		StringViewArgument( debug_name ),
		*id
	);
#endif
}

static void
opengl_create_texture_2d( GLuint *id, StringView_ASCII debug_name ) {
	glCreateTextures( GL_TEXTURE_2D, 1, id );
#ifdef QLIGHT_DEBUG
	if ( debug_name.size > 0 )
		glObjectLabel( GL_TEXTURE, *id, debug_name.size, debug_name.data );
	log_debug_gl( "Created 2D Texture '" StringViewFormat "' (gl_id: %u).",
		StringViewArgument( debug_name ),
		*id
	);
#endif
}

bool
renderer_texture_2d_upload(
	Texture_ID texture_id,
	Vector2_u16 origin,
	Vector2_u16 dimensions,
	u8 mipmap_levels,
	GLint opengl_storage_format,
	GLenum opengl_pixel_type
) {
	Texture *texture = texture_instance( texture_id );
	if ( texture->opengl_id != 0 )
		return false;

	texture->origin = origin;
	texture->dimensions = dimensions;
	texture->mipmap_levels = mipmap_levels;
	texture->opengl_storage_format = opengl_storage_format;
	texture->opengl_pixel_type = opengl_pixel_type;

	// 1. Create OpenGL identifier.
	opengl_create_texture_2d( &texture->opengl_id, texture->name );

	// 2. Allocate Immutable Texture Storage.
	// It cannot be modified (reused) like the legacy one, a new one must be created.
	glTextureStorage2D(
		/*        texture */ texture->opengl_id,
		/*         levels */ texture->mipmap_levels,
		/* internalformat */ texture->opengl_storage_format, // Sized format, for example: `GL_RGBA8`.
		/*          width */ ( GLsizei )texture->dimensions.width,
		/*         height */ ( GLsizei )texture->dimensions.height
	);

#ifdef QLIGHT_DEBUG
	// Make sure it actually uses Immutable Texture Storage.
	GLint opengl_texture_storage_immutable = 0;
	glGetTextureParameteriv( texture->opengl_id, GL_TEXTURE_IMMUTABLE_FORMAT, &opengl_texture_storage_immutable );
	Assert( opengl_texture_storage_immutable == GL_TRUE );
#endif

	// 3. Upload texture data.
	if ( texture->bytes.data ) {
		GLenum opengl_format = renderer_texture_channels_to_opengl( texture->channels );
		glTextureSubImage2D(
			/* texture */ texture->opengl_id,
			/*   level */ 0,  // 0th mipmap - Base image, full resolution.
			/* xoffset */ ( GLint )texture->origin.x,
			/* yoffset */ ( GLint )texture->origin.y,
			/*   width */ ( GLsizei )texture->dimensions.width,
			/*  height */ ( GLsizei )texture->dimensions.height,
			/*  format */ opengl_format, // Generic channels layout format, for example: `GL_RGB`.
			/*    type */ texture->opengl_pixel_type,  // Pixel data type, for example: `GL_UNSIGNED_BYTE`.
			/*   pixel */ texture->bytes.data
		);

		// 4. Generate mipmaps automatically from the base level.
		if ( texture->mipmap_levels > 1 )
			glGenerateTextureMipmap( texture->opengl_id );
	}

	// 5. Set texture parameters.
	glTextureParameteri( texture->opengl_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTextureParameteri( texture->opengl_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	// glTextureParameteri( texture->opengl_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
	// glTextureParameteri( texture->opengl_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );

	StringView_ASCII storage_format_name = opengl_storage_format_name( texture->opengl_storage_format );
	StringView_ASCII pixel_type_name = opengl_pixel_type_name( texture->opengl_pixel_type );
	log_debug(
		"Uploaded 2D Texture '" StringViewFormat "' (#%u, %u bytes) to GPU memory (%hux%hu, origin: %hux%hu, %hhu mips, " StringViewFormat ", " StringViewFormat ", gl_id: %u).",
		StringViewArgument( texture->name ),
		texture_id,
		texture->bytes.size,
		texture->dimensions.width,
		texture->dimensions.height,
		texture->origin.x,
		texture->origin.y,
		texture->mipmap_levels,
		StringViewArgument( storage_format_name ),
		StringViewArgument( pixel_type_name ),
		texture->opengl_id
	);
	return true;
}

bool
renderer_mesh_upload( Mesh_ID mesh_id ) {
	Mesh *mesh = mesh_instance( mesh_id );

	if ( mesh->opengl_vao != 0 || mesh->opengl_vbo != 0 || mesh->opengl_ebo != 0 ) {
		printf(
			"WARNING: mesh_upload_to_renderer: Trying to upload already uploaded mesh (id: %u, name: \"" StringViewFormat "\"). Skipping.\n",
			mesh_id,
			StringViewArgument( mesh->name )
		);
		return false;
	}

	/* 1. Create Vertex Array Object */

	opengl_create_and_bind_vertex_array( &mesh->opengl_vao, string_view( &mesh->name ) );

	/* 2. Create Vertex Buffer Object */

	opengl_create_and_bind_vertex_buffer( &mesh->opengl_vbo, string_view( &mesh->name ) );

	/* 3. Upload Vertex data */

	GLenum opengl_vertex_buffer_usage = ( mesh_is_dynamic( mesh ) ) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
	u32 vertices_size = mesh->vertices.size * sizeof( Vertex_3D );
	u8 *vertices_data = mesh->vertices.data;
	glBufferData( GL_ARRAY_BUFFER, vertices_size, vertices_data, opengl_vertex_buffer_usage );

	/* 4. Define vertex attributes */

	/*
		WARNING: Vertex attributes offsets are set sequentially!
		If attribute indices do not correspond to indices within array, offsets will be completely wrong.
		TODO: Do something about it.
	*/
	u64 offset = 0;
	u64 attributes_size = mesh_vertex_attributes_size( mesh );
	ForIt( mesh->vertex_attributes.data, mesh->vertex_attributes.size ) {
		if ( !it.active )
			continue;

		GLenum opengl_data_type = renderer_data_type_to_opengl_type( it.data_type );
		u32 data_type_size = ( u32 )renderer_data_type_size( it.data_type ) * it.elements;
		glVertexAttribPointer( it.index, it.elements, opengl_data_type, it.normalize, ( GLsizei )attributes_size, ( void * )offset );
		glEnableVertexAttribArray( it.index );
		offset += data_type_size;
	}}

	/* 5. Create Element Buffer Object */

	opengl_create_and_bind_element_buffer( &mesh->opengl_ebo, string_view( &mesh->name ) );

	// 5.3. Allocate memory and pass data
	GLenum opengl_index_buffer_usage = ( mesh_is_dynamic( mesh ) ) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
	u32 indices_size = mesh->indices.size * mesh->indices.item_size;
	u8 *indices_data = mesh->indices.data;
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices_size, indices_data, opengl_index_buffer_usage );

	log_debug( "Uploaded Mesh '" StringViewFormat "' (#%u, %u vertices, %u indices) to GPU memory (VAO: %u, VBO: %u, EBO: %u).",
		StringViewArgument( mesh->name ),
		mesh_id,
		mesh->vertices.size,
		mesh->indices.size,
		mesh->opengl_vao,
		mesh->opengl_vbo,
		mesh->opengl_ebo
	);
	return true;
}

bool
renderer_model_meshes_upload( Model_ID model_id ) {
	Model *model = model_instance( model_id );
	bool mesh_uploaded;
	ForIt( model->meshes.data, model->meshes.size ) {
		mesh_uploaded = renderer_mesh_upload( it );
		if ( !mesh_uploaded )
			return false;
	}}

	return true;
}

GLenum
renderer_texture_channels_to_opengl( Texture_Channels channels ) {
	switch ( channels ) {
		case TextureChannels_Red:           return GL_RED;
		case TextureChannels_RG:            return GL_RG;
		case TextureChannels_RGB:           return GL_RGB;
		case TextureChannels_RGBA:          return GL_RGBA;

		case TextureChannels_BGR:           return GL_BGR;
		case TextureChannels_BGRA:          return GL_BGRA;

		case TextureChannels_Depth:         return GL_DEPTH_COMPONENT;
		case TextureChannels_Stencil:       return GL_STENCIL_INDEX;
		case TextureChannels_DepthStencil:  return GL_DEPTH_STENCIL;

		case TextureChannels_None:
		default:                          return GL_INVALID_ENUM;
	}
}

StringView_ASCII renderer_device_vendor() {
	return g_renderer.device.vendor;
}

StringView_ASCII renderer_device_name() {
	return g_renderer.device.name;
}

void
renderer_set_clear_color( Vector4_f32 color ) {
	g_renderer.clear_color = color;
}

Vector4_f32
renderer_get_clear_color() {
	return g_renderer.clear_color;
}

Texture_ID
renderer_texture_white() {
	return g_renderer.texture_white;
}

Texture_ID
renderer_texture_black() {
	return g_renderer.texture_black;
}

Texture_ID
renderer_texture_purple_checkers() {
	return g_renderer.texture_purple_checkers;
}

static void
opengl_create_and_bind_uniform_buffer( GLuint *id, GLsizeiptr size, GLbitfield storage_bits, StringView_ASCII debug_name ) {
	glGenBuffers( 1, id );
	glBindBuffer( GL_UNIFORM_BUFFER, *id );
#ifdef QLIGHT_DEBUG
	if ( debug_name.size > 0 )
		glObjectLabel( GL_BUFFER, *id, debug_name.size, debug_name.data );
#endif
	log_debug_gl( "Created and bound Uniform Buffer '" StringViewFormat "' (gl_id: %u).",
		StringViewArgument( debug_name ),
		*id
	);
	glNamedBufferStorage(
		/* buffer */ *id,
		/*   size */ size,
		/*   data */ NULL,
		/*  flags */ storage_bits
	);
}

Renderer_Uniform_Buffer *
renderer_uniform_buffer_create( StringView_ASCII name, u32 size, Renderer_GL_Buffer_Storage_Bits storage_bits, u32 binding ) {
	Renderer_Uniform_Buffer _uniform_buffer = {
		.name = name,
		.size = size,
		.binding = binding,
		.storage_bits = storage_bits
		// .opengl_ubo
	};

	opengl_create_and_bind_uniform_buffer( &_uniform_buffer.opengl_ubo, size, storage_bits, name );

	u32 buffer_idx = array_add( &g_renderer.uniform_buffers, _uniform_buffer );
	Renderer_Uniform_Buffer *uniform_buffer = &g_renderer.uniform_buffers.data[ buffer_idx ];

	if ( binding != INVALID_UNIFORM_BUFFER_BINDING )
		renderer_uniform_buffer_set_binding( uniform_buffer, binding );

	return uniform_buffer;
}

void
renderer_uniform_buffer_set_binding( Renderer_Uniform_Buffer *uniform_buffer, u32 binding ) {
	glBindBuffer( GL_UNIFORM_BUFFER, uniform_buffer->opengl_ubo );
	glBindBufferBase(
		/* target */ GL_UNIFORM_BUFFER,
		/* index  */ binding,
		/* buffer */ uniform_buffer->opengl_ubo
	);
	uniform_buffer->binding = binding;
}

void *
renderer_uniform_buffer_memory_map(
	Renderer_Uniform_Buffer *uniform_buffer,
	Renderer_GL_Map_Access_Bits access_bits,
	u32 size,
	u32 offset
) {
	u32 length = ( size != 0 ) ? size : uniform_buffer->size - offset;
	Assert( offset < uniform_buffer->size );
	void *uniform_data = glMapNamedBufferRange(
		/* buffer */ uniform_buffer->opengl_ubo,
		/* offset */ offset,
		/* length */ length,
		/* access */ access_bits
	);

	return uniform_data;
}

u32
renderer_uniform_buffer_write( Renderer_Uniform_Buffer *uniform_buffer, ArrayView< u8 > write_data, u32 size, u32 offset ) {
	if ( !write_data.data || write_data.size == 0 )
		return 0;

	Assert( size <= uniform_buffer->size - offset );
	u32 write_size = size;
	if ( size == 0 )
		write_size = uniform_buffer->size - offset;


	Renderer_GL_Map_Access_Bits access_bits = renderer_gl_buffer_storage_bits_to_map_access_bits( uniform_buffer->storage_bits );
	void *uniform_data = renderer_uniform_buffer_memory_map( uniform_buffer, access_bits, write_size, offset );
	memcpy( uniform_data, write_data.data, write_size );
	Assert( renderer_uniform_buffer_memory_unmap( uniform_buffer, access_bits, size, offset ) );
	return write_size;
}

u32
renderer_uniform_buffer_read( Renderer_Uniform_Buffer *uniform_buffer, Array< u8 > *out_read_data, u32 size, u32 offset ) {
	if ( !out_read_data )
		return 0;

	Assert( size <= uniform_buffer->size - offset );
	ArrayView< u8 > read_view;
	read_view.size = size;
	if ( size == 0 )
		read_view.size = uniform_buffer->size - offset;

	Renderer_GL_Map_Access_Bits access_bits = renderer_gl_buffer_storage_bits_to_map_access_bits( uniform_buffer->storage_bits );
	read_view.data = ( u8 * )renderer_uniform_buffer_memory_map( uniform_buffer, access_bits, size, offset );
	u32 bytes_read = array_add_many( out_read_data, read_view );
	Assert( renderer_uniform_buffer_memory_unmap( uniform_buffer, access_bits, size, offset ) );
	return bytes_read;
}

bool
renderer_uniform_buffer_memory_unmap( Renderer_Uniform_Buffer *uniform_buffer, Renderer_GL_Map_Access_Bits access_bits, u32 size, u32 offset ) {
	// TODO: Handle specific cases based on access bits:
	//   InvalidateRange,
	//   InvalidateBuffer,
	//   FlushExplicit,
	//   Unsynchronized.

	// u32 length = ( size != 0 ) ? size : uniform_buffer->size - offset;
	// GL_ASSERT( glFlushMappedNamedBufferRange(
	// 	/* buffer */ uniform_buffer->opengl_ubo,
	// 	/* offset */ ( GLintptr )offset,
	// 	/* length */ ( GLsizei )length
	// ) );
	bool result = glUnmapNamedBuffer( uniform_buffer->opengl_ubo );
	return result;
}

bool
renderer_uniform_buffer_destroy( Renderer_Uniform_Buffer *uniform_buffer ) {
	return false;
}

void
renderer_set_camera_position_pointer( Vector3_f32 *camera_position ) {
	g_renderer.camera_position = camera_position;
}

void
renderer_set_ambient_light_color( Vector3_f32 ambient_light ) {
	g_renderer.ambient_light = ambient_light;
}

StringView_ASCII
renderer_framebuffer_attachment_point_name( Renderer_Framebuffer_Attachment_Point attachment_point ) {
	switch ( attachment_point ) {
		case RendererFramebufferAttachmentPoint_Color0: return "Color0";
		case RendererFramebufferAttachmentPoint_Color1: return "Color1";
		case RendererFramebufferAttachmentPoint_Color2: return "Color2";
		case RendererFramebufferAttachmentPoint_Color3: return "Color3";
		case RendererFramebufferAttachmentPoint_Color4: return "Color4";
		case RendererFramebufferAttachmentPoint_Color5: return "Color5";
		case RendererFramebufferAttachmentPoint_Color6: return "Color6";
		case RendererFramebufferAttachmentPoint_Color7: return "Color7";
		case RendererFramebufferAttachmentPoint_Color8: return "Color8";
		case RendererFramebufferAttachmentPoint_Color9: return "Color9";
		case RendererFramebufferAttachmentPoint_Color10: return "Color10";
		case RendererFramebufferAttachmentPoint_Color11: return "Color11";
		case RendererFramebufferAttachmentPoint_Color12: return "Color12";
		case RendererFramebufferAttachmentPoint_Color13: return "Color13";
		case RendererFramebufferAttachmentPoint_Color14: return "Color14";
		case RendererFramebufferAttachmentPoint_Color15: return "Color15";

		case RendererFramebufferAttachmentPoint_Depth: return "Depth";
		case RendererFramebufferAttachmentPoint_Stencil: return "Stencil";
		case RendererFramebufferAttachmentPoint_DepthStencil: return "DepthStencil";

		case RendererFramebufferAttachmentPoint_None:
		default:                                  return ( const char *)NULL;
	}
}

f32
renderer_frame_time_delta() {
	return g_renderer.frame_time.delta;
}

void
renderer_set_output_channel( Renderer_Output_Channel channel ) {
	g_renderer.output_channel = channel;
}
