#ifndef QLIGHT_RENDERER_H
#define QLIGHT_RENDERER_H

// #define GLEW_STATIC - Defined in project settings.
#include "../libs/GLEW/glew.h"
#include "../libs/GLFW/glfw3.h" // TODO: remove

#include "common.h"
#include "string.h"
#include "math.h"
#include "model.h"
#include "transform.h"
#include "texture.h"

constexpr float NEAR_CLIP_PLANE_DISTANCE = 0.01f;
constexpr float FAR_CLIP_PLANE_DISTANCE = 200.0f;
constexpr float PITCH_MAX_ANGLE = 89.0f;

enum Renderer_Shader_Kind : u32 {
	RendererShaderKind_Vertex = 0,
	RendererShaderKind_TesselationControl,
	RendererShaderKind_TesselationEvaluation,
	RendererShaderKind_Geometry,
	RendererShaderKind_Fragment,
	RendererShaderKind_Compute,

	RendererShaderKind_COUNT
};

enum ERenderer_Shader_Kind_Bits : u32 {
	RendererShaderKindBit_Vertex                 = ( 1 << 0 ),
	RendererShaderKindBit_TesselationControl     = ( 1 << 1 ),
	RendererShaderKindBit_TesselationEvaluation  = ( 1 << 2 ),
	RendererShaderKindBit_Geometry               = ( 1 << 3 ),
	RendererShaderKindBit_Fragment               = ( 1 << 4 ),
	RendererShaderKindBit_Compute                = ( 1 << 5 )
};
typedef u32 Renderer_Shader_Kind_Bits;

enum Renderer_Data_Type : u32 {
	RendererDataType_s8 = 0,
	RendererDataType_u8,
	RendererDataType_s16,
	RendererDataType_u16,
	RendererDataType_s32,
	RendererDataType_u32,

	RendererDataType_f16,
	RendererDataType_f32,
	RendererDataType_f64,

	RendererDataType_Vector2_f32,
	RendererDataType_Vector3_f32,
	RendererDataType_Vector4_f32,

	RendererDataType_Matrix2x2_f32,
	RendererDataType_Matrix2x3_f32,
	RendererDataType_Matrix2x4_f32,

	RendererDataType_Matrix3x2_f32,
	RendererDataType_Matrix3x3_f32,
	RendererDataType_Matrix3x4_f32,

	RendererDataType_Matrix4x2_f32,
	RendererDataType_Matrix4x3_f32,
	RendererDataType_Matrix4x4_f32,

	RendererDataType_COUNT
};

struct Renderer_Shader_Stage {
	StringView_ASCII name;
	StringView_ASCII file_path;
	String_ASCII source_code;

	Renderer_Shader_Kind kind;

	// OpenGL-specific:
	GLuint opengl_shader;
};

struct Renderer_Vertex_Attribute {
	StringView_ASCII name;
	u32 index;
	u32 elements; // 1, 2, 3, or 4
	Renderer_Data_Type data_type;
	bool normalize;
	bool active;
};

struct Renderer_Uniform {
	StringView_ASCII name;
	Renderer_Data_Type data_type;

	// OpenGL-specific:
	GLint opengl_location;
};

typedef void UBO_Struct;
struct Renderer_Uniform_Buffer {
	StringView_ASCII name;
	UBO_Struct *data;
	u32 size;
	u32 binding;

	// OpenGL-specific:
	GLuint opengl_ubo;
};
constexpr u32 INVALID_UNIFORM_BUFFER_BINDING = U32_MAX;

struct Renderer_Shader_Program {
	StringView_ASCII name;
	Renderer_Shader_Stage      *shaders[ RendererShaderKind_COUNT ]; // stages
	Renderer_Shader_Kind_Bits  linked_shaders;

	Array< Renderer_Vertex_Attribute > vertex_attributes;
	Array< Renderer_Uniform > uniforms;

	// OpenGL-specific:
	GLuint opengl_program;
};

struct Renderer_Pipeline {
	// Renderer_Shader

	// OpenGL-specific:
	GLuint opengl_pipeline;
};

enum Renderer_Framebuffer_Attachment : u16 {
	RendererFramebufferAttachment_Color0 = 0,
	RendererFramebufferAttachment_Color1,
	RendererFramebufferAttachment_Color2,
	RendererFramebufferAttachment_Color3,
	RendererFramebufferAttachment_Color4,
	RendererFramebufferAttachment_Color5,
	RendererFramebufferAttachment_Color6,
	RendererFramebufferAttachment_Color7,
	RendererFramebufferAttachment_Color8,
	RendererFramebufferAttachment_Color9,
	RendererFramebufferAttachment_Color10,
	RendererFramebufferAttachment_Color11,
	RendererFramebufferAttachment_Color12,
	RendererFramebufferAttachment_Color13,
	RendererFramebufferAttachment_Color14,
	RendererFramebufferAttachment_Color15,

	RendererFramebufferAttachment_Depth,
	RendererFramebufferAttachment_Stencil,
	RendererFramebufferAttachment_DepthStencil,

	RendererFramebufferAttachment_None = U16_MAX
};

struct Renderer_Framebuffer {
	StringView_ASCII name;
	GLuint opengl_framebuffer;
};
typedef u8 Renderer_Framebuffer_ID;
constexpr Renderer_Framebuffer_ID INVALID_FRAMEBUFFER_ID = U8_MAX;

struct Renderer_Renderbuffer {
	StringView_ASCII name;
	Renderer_Framebuffer_Attachment attachment;
	GLuint opengl_renderbuffer;
	// Stored in OpenGL (glGetRenderbufferParameter):
	// width
	// height
	// internal_format
	// samples
	// R G B A | D S component sizes
};
typedef u16 Renderer_Renderbuffer_ID;
constexpr Renderer_Renderbuffer_ID INVALID_RENDERBUFFER_ID = U16_MAX;

struct Renderer_Render_Command {
	Mesh_ID mesh_id;
	Material_ID material_id;
	Transform *transform;
};

struct Vertex_3D {
	Vector3_f32 position;
	Vector3_f32 normal;
	Vector2_f32 texture_uv;
	// Vector3_f32 texture_uvw;
};

bool renderer_init();
void renderer_shutdown();

Renderer_Shader_Stage *
renderer_find_shader_stage( StringView_ASCII name );

Renderer_Shader_Program *
renderer_find_shader_program( StringView_ASCII name );

bool
renderer_destroy_shader_stage( Renderer_Shader_Stage *stage );

bool
renderer_destroy_shader_program( Renderer_Shader_Program *program );

StringView_ASCII
renderer_shader_kind_name( Renderer_Shader_Kind kind );

Renderer_Shader_Kind_Bits
renderer_shader_kind_bit( Renderer_Shader_Kind kind );

GLenum
renderer_shader_kind_to_opengl( Renderer_Shader_Kind kind );

Renderer_Shader_Stage *
// renderer_load_shader_stage( StringView_ASCII name, Renderer_Shader_Kind kind, StringView_UTF8 file_path );
renderer_load_shader_stage( StringView_ASCII name, Renderer_Shader_Kind kind, StringView_ASCII file_path );

Renderer_Shader_Program *
renderer_create_and_compile_shader_program( StringView_ASCII name, ArrayView< Renderer_Shader_Stage * > shader_stages );

bool
renderer_shader_program_set_uniform( Renderer_Shader_Program *program, StringView_ASCII uniform_name, Renderer_Data_Type data_type, void *value );

u32
renderer_shader_program_update_uniform_locations( Renderer_Shader_Program *program );

void
renderer_set_uniforms_transpose_matrix( bool value );

u64
renderer_data_type_size( Renderer_Data_Type data_type );

GLenum
renderer_data_type_to_opengl_type( Renderer_Data_Type data_type );

Renderer_Renderbuffer_ID
renderer_create_renderbuffer(
	StringView_ASCII name,
	u32 width,
	u32 height,
	Renderer_Framebuffer_Attachment attachment,
	GLenum opengl_internal_format
);

Renderer_Renderbuffer_ID
renderer_find_renderbuffer( StringView_ASCII name );

Renderer_Renderbuffer *
renderer_renderbuffer_instance( Renderer_Renderbuffer_ID renderbuffer_id );

Renderer_Framebuffer_ID
renderer_create_framebuffer( StringView_ASCII name );

Renderer_Framebuffer_ID
renderer_find_framebuffer( StringView_ASCII name );

Renderer_Framebuffer *
renderer_framebuffer_instance( Renderer_Framebuffer_ID framebuffer_id );

bool
renderer_attach_renderbuffer_to_framebuffer( Renderer_Framebuffer_ID framebuffer_id, Renderer_Renderbuffer_ID renderbuffer_id );

GLenum
renderer_framebuffer_attachment_to_opengl( Renderer_Framebuffer_Attachment attachment );

bool
renderer_is_framebuffer_complete( Renderer_Framebuffer_ID framebuffer_id );

void
renderer_set_active_framebuffer_color_attachments( Renderer_Framebuffer_ID framebuffer_id, ArrayView< Renderer_Framebuffer_Attachment > color_attachments );

void
renderer_draw_frame();

bool
renderer_bind_framebuffer( Renderer_Framebuffer_ID framebuffer_id );

bool
renderer_bind_shader_program( Renderer_Shader_Program *program );

bool
renderer_bind_texture( u32 texture_slot_idx, Texture_ID texture_id );

void
renderer_queue_draw_command( Mesh_ID mesh_id, Material_ID material_id, Transform *transform );

void
renderer_set_view_matrix_pointer( Matrix4x4_f32 *view );

void
renderer_set_projection_matrix_pointer( Matrix4x4_f32 *projection );

Renderer_Framebuffer_Attachment
renderer_texture_format_to_framebuffer_attachment( Texture_Format format );

bool
renderer_texture_attach_to_framebuffer( Texture_ID texture_id, Renderer_Framebuffer_ID framebuffer_id, Renderer_Framebuffer_Attachment attachment );

bool
renderer_texture_upload( Texture_ID texture_id );

bool
renderer_mesh_upload( Mesh_ID mesh_id );

bool
renderer_model_meshes_upload( Model_ID model_id );

GLenum
renderer_texture_format_to_opengl( Texture_Format format );

StringView_ASCII
renderer_device_vendor();

StringView_ASCII
renderer_device_name();

void
renderer_set_clear_color( Vector4_f32 color );

Vector4_f32
renderer_get_clear_color();

Texture_ID
renderer_texture_white();

Texture_ID
renderer_texture_black();

Texture_ID
renderer_texture_purple_checkers();

Renderer_Uniform_Buffer *
renderer_uniform_buffer_create( StringView_ASCII name, UBO_Struct *data, u32 size, u32 binding = INVALID_UNIFORM_BUFFER_BINDING );

void
renderer_uniform_buffer_set_binding( Renderer_Uniform_Buffer *uniform_buffer, u32 binding );

bool
renderer_uniform_buffer_destroy( Renderer_Uniform_Buffer *uniform_buffer );

void
renderer_set_camera_position_pointer( Vector3_f32 *camera_position );

void
renderer_set_ambient_light_color( Vector3_f32 ambient_light );

StringView_ASCII
renderer_framebuffer_attachment_name( Renderer_Framebuffer_Attachment attachment );

f32
renderer_frame_time_delta();

#endif /* QLIGHT_RENDERER_H */
