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

enum Renderer_GL_Buffer_Usage : GLenum {
	RendererGLBufferUsage_None = 0,

	RendererGLBufferUsage_StreamDraw = GL_STREAM_DRAW,
	RendererGLBufferUsage_StreamRead = GL_STREAM_READ,
	RendererGLBufferUsage_StreamCopy = GL_STREAM_COPY,

	RendererGLBufferUsage_StaticDraw = GL_STATIC_DRAW,
	RendererGLBufferUsage_StaticRead = GL_STATIC_READ,
	RendererGLBufferUsage_StaticCopy = GL_STATIC_COPY,

	RendererGLBufferUsage_DynamicDraw = GL_DYNAMIC_DRAW,
	RendererGLBufferUsage_DynamicRead = GL_DYNAMIC_READ,
	RendererGLBufferUsage_DynamicCopy = GL_DYNAMIC_COPY
};

// These forward declarations are needed for bits casting functions down below.
enum Renderer_GL_Map_Access_Bits;
enum Renderer_GL_Buffer_Storage_Bits;

// More info at: `docs.gl/gl4/glMapBufferRange`
enum Renderer_GL_Map_Access_Bits : GLbitfield {
	RendererGLMapAccessBit_None = 0,

	// GL_MAP_READ_BIT indicates that the returned pointer may be used
	//   to read buffer object data.
	// No GL error is generated if the pointer is used to query a mapping
	//   which excludes this flag, but the result is undefined and system errors
	//   (possibly including program termination) may occur.
	RendererGLMapAccessBit_Read = GL_MAP_READ_BIT,

	// GL_MAP_WRITE_BIT indicates that the returned pointer may be used
	//   to modify buffer object data.
	// No GL error is generated if the pointer is used to modify a mapping
	//   which excludes this flag, but the result is undefined and system errors
	//   (possibly including program termination) may occur.
	RendererGLMapAccessBit_Write = GL_MAP_WRITE_BIT,

	// GL_MAP_PERSISTENT_BIT indicates that the mapping is to be made
	//   in a persistent fassion and that the client intends to hold
	//   and use the returned pointer during subsequent GL operation.
	// It is not an error to call drawing commands (render) while buffers
	//   are mapped using this flag.
	// It is an error to specify this flag if the buffer's data store
	//   was not allocated through a call to the glBufferStorage command
	//   in which the GL_MAP_PERSISTENT_BIT was also set.
	RendererGLMapAccessBit_Persistent = GL_MAP_PERSISTENT_BIT,

	// GL_MAP_COHERENT_BIT indicates that a persistent mapping is also to be coherent.
	// Coherent maps guarantee that the effect of writes to a buffer's data store
	//   by either the client or server will eventually become visible to the other
	//   without further intervention from the application.
	// In the absence of this bit, persistent mappings are not coherent
	//   and modified ranges of the buffer store must be explicitly communicated
	//   to the GL, either by unmapping the buffer, or through a call to
	//   glFlushMappedBufferRange or glMemoryBarrier.
	RendererGLMapAccessBit_Coherent = GL_MAP_COHERENT_BIT,

	// --- Optional ---

	// GL_MAP_INVALIDATE_RANGE_BIT indicates that the previous contents
	//   of the specified range may be discarded.
	// Data within this range are undefined with the exception of subsequently written data.
	// No GL error is generated if subsequent GL operations access unwritten data,
	//   but the result is undefined and system errors
	//   (possibly including program termination) may occur.
	// This flag may not be used in combination with GL_MAP_READ_BIT.
	RendererGLMapAccessBit_InvalidateRange = GL_MAP_INVALIDATE_RANGE_BIT,

	// GL_MAP_INVALIDATE_BUFFER_BIT indicates that the previous contents
	//   of the entire buffer may be discarded.
	// Data within the entire buffer are undefined with the exception of subsequently written data.
	// No GL error is generated if subsequent GL operations access unwritten data,
	//   but the result is undefined and system errors
	//   (possibly including program termination) may occur.
	// This flag may not be used in combination with GL_MAP_READ_BIT.
	RendererGLMapAccessBit_InvalidateBuffer = GL_MAP_INVALIDATE_BUFFER_BIT,

	// GL_MAP_FLUSH_EXPLICIT_BIT indicates that one or more discrete subranges
	//   of the mapping may be modified.
	// When this flag is set, modifications to each subrange must be
	//   explicitly flushed by calling glFlushMappedBufferRange.
	// No GL error is set if a subrange of the mapping is modified and not flushed,
	//   but data within the corresponding subrange of the buffer are undefined.
	// This flag may only be used in conjunction with GL_MAP_WRITE_BIT.
	// When this option is selected, flushing is strictly limited to regions
	//   that are explicitly indicated with calls to glFlushMappedBufferRange prior to unmap.
	// If this option is not selected glUnmapBuffer will automatically flush the entire mapped range when called.
	RendererGLMapAccessBit_FlushExplicit = GL_MAP_FLUSH_EXPLICIT_BIT,

	// GL_MAP_UNSYNCHRONIZED_BIT indicates that the GL should not attempt
	//   to synchronize pending operations on the buffer prior to returning
	//   from glMapBufferRange or glMapNamedBufferRange.
	// No GL error is generated if pending operations which source or modify
	//   the buffer overlap the mapped region, but the result of such
	//   previous and any subsequent operations is undefined.
	RendererGLMapAccessBit_Unsynchronized = GL_MAP_UNSYNCHRONIZED_BIT
};

constexpr Renderer_GL_Map_Access_Bits RendererGLMapAccessBits_ReadWrite = static_cast< const Renderer_GL_Map_Access_Bits >(
	RendererGLMapAccessBit_Read |
	RendererGLMapAccessBit_Write );

constexpr Renderer_GL_Map_Access_Bits RendererGLMapAccessBits_SharedWithBufferStorage = static_cast< const Renderer_GL_Map_Access_Bits >(
	RendererGLMapAccessBits_ReadWrite |
	RendererGLMapAccessBit_Persistent |
	RendererGLMapAccessBit_Coherent );

inline Renderer_GL_Buffer_Storage_Bits
renderer_gl_map_access_bits_to_buffer_storage_bits( Renderer_GL_Map_Access_Bits access_bits ) {
	return static_cast< Renderer_GL_Buffer_Storage_Bits >( access_bits & RendererGLMapAccessBits_SharedWithBufferStorage );
}

inline Renderer_GL_Map_Access_Bits
renderer_gl_map_access_bits( GLbitfield access_bits ) {
	return static_cast< Renderer_GL_Map_Access_Bits >( access_bits );
}

// More info at: `https://docs.gl/gl4/glBufferStorage`
enum Renderer_GL_Buffer_Storage_Bits : GLbitfield {
	RendererGLBufferStorageBit_None = 0,

	// The contents of the data store may be updated after creation through
	//   calls to glBufferSubData.
	// If this bit is not set, the buffer content may not be directly updated
	//   by the client. The data argument may be used to specify the initial
	//   content of the buffer's data store regardless of the presence of the
	//   GL_DYNAMIC_STORAGE_BIT.
	// Regardless of the presence of this bit, buffers may always be updated
	//   with server-side calls such as glCopyBufferSubData and glClearBufferSubData.
	RendererGLBufferStorageBit_DynamicStorage = GL_DYNAMIC_STORAGE_BIT,

	// --- Shared with Map Access Bits BEGIN ---

	// The data store may be mapped by the client for read access and a pointer
	//   in the client's address space obtained that may be read from.
	RendererGLBufferStorageBit_Read = GL_MAP_READ_BIT,

	// The data store may be mapped by the client for write access and a pointer
	//   in the client's address space obtained that may be written through.
	RendererGLBufferStorageBit_Write = GL_MAP_WRITE_BIT,

	// The client may request that the server read from or write to the buffer
	//   while it is mapped.
	// The client's pointer to the data store remains valid so long as
	//   the data store is mapped, even during execution of drawing or dispatch commands.
	RendererGLBufferStorageBit_Persistent = GL_MAP_PERSISTENT_BIT,

	// Shared access to buffers that are simultaneously mapped for client access
	//   and are used by the server will be coherent, so long as that mapping
	//   is performed using glMapBufferRange.
	// That is, data written to the store by either the client or server will be
	//   immediately visible to the other with no further action taken by the application.
	// In particular,
	//   * If GL_MAP_COHERENT_BIT is not set and the client performs a write
	//       followed by a call to the glMemoryBarrier command with the
	//       GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT set, then in subsequent commands
	//       the server will see the writes.
	//   * If GL_MAP_COHERENT_BIT is set and the client performs a write,
	//       then in subsequent commands the server will see the writes.
	//   * If GL_MAP_COHERENT_BIT is not set and the server performs a write,
	//       the application must call glMemoryBarrier with the
	//       GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT set and then
	//       call glFenceSync with GL_SYNC_GPU_COMMANDS_COMPLETE (or glFinish).
	//     Then the CPU will see the writes after the sync is complete.
	//   * If GL_MAP_COHERENT_BIT is set and the server does a write,
	//       the app must call FenceSync with GL_SYNC_GPU_COMMANDS_COMPLETE (or glFinish).
	//     Then the CPU will see the writes after the sync is complete.
	RendererGLBufferStorageBit_Coherent = GL_MAP_COHERENT_BIT,

	// --- Shared with Map Access Bits END ---

	// When all other criteria for the buffer storage allocation are met,
	//   this bit may be used by an implementation to determine whether to
	//   use storage that is local to the server or to the client to serve
	//   as the backing store for the buffer.
	RendererGLBufferStorageBit_ClientStorage = GL_CLIENT_STORAGE_BIT

	// glBufferStorage, glNamedBufferStorage:
	// The allowed combinations of flags are subject to certain restrictions.
	// They are as follows:
	//   * If flags contains GL_MAP_PERSISTENT_BIT, it must also contain
	//       at least one of GL_MAP_READ_BIT or GL_MAP_WRITE_BIT.
	//   * If flags contains GL_MAP_COHERENT_BIT, it must also contain
	//       GL_MAP_PERSISTENT_BIT.
};

constexpr Renderer_GL_Buffer_Storage_Bits RendererGLBufferStorageBits_ReadWrite = static_cast< const Renderer_GL_Buffer_Storage_Bits >(
	RendererGLBufferStorageBit_Read |
	RendererGLBufferStorageBit_Write );

constexpr Renderer_GL_Buffer_Storage_Bits RendererGLBufferStorageBits_SharedWithMapAccess = static_cast< const Renderer_GL_Buffer_Storage_Bits >(
	RendererGLBufferStorageBits_ReadWrite |
	RendererGLBufferStorageBit_Persistent |
	RendererGLBufferStorageBit_Coherent );

inline Renderer_GL_Map_Access_Bits
renderer_gl_buffer_storage_bits_to_map_access_bits( Renderer_GL_Buffer_Storage_Bits storage_bits ) {
	return static_cast< Renderer_GL_Map_Access_Bits >( storage_bits & RendererGLBufferStorageBits_SharedWithMapAccess );
}

inline Renderer_GL_Buffer_Storage_Bits
renderer_gl_buffer_storage_bits( GLbitfield storage_bits ) {
	return static_cast< Renderer_GL_Buffer_Storage_Bits >( storage_bits );
}

enum Renderer_GL_Buffer_Access_Mode : GLenum {
	RendererGLBufferAccessMode_None = 0,

	RendererGLBufferAccessMode_Read = GL_READ_ONLY,
	RendererGLBufferAccessMode_Write = GL_WRITE_ONLY,
	RendererGLBufferAccessMode_ReadWrite = GL_READ_WRITE
};

struct Renderer_GL_Buffer_Mapping {
	ArrayView< u8 > view;
	u32 offset;
	Renderer_GL_Map_Access_Bits access_bits;

	// Maybe store pointer to a `Renderer_GL_Buffer` object?
	GLuint opengl_buffer_id;
};

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
	u32 size;
	u32 binding;
	Renderer_GL_Buffer_Storage_Bits storage_bits;

	// OpenGL-specific:
	GLuint opengl_ubo;
};
constexpr u32 INVALID_UNIFORM_BUFFER_BINDING = U32_MAX;

struct Renderer_Shader_Program {
	StringView_ASCII name;
	Renderer_Shader_Stage      *shaders[ RendererShaderKind_COUNT ]; // stages
	Renderer_Shader_Kind_Bits  linked_shaders;

	Array< Renderer_Vertex_Attribute > vertex_attributes; // Shader-bound declaration info. Read only.
	Array< Renderer_Uniform > uniforms;
	Array< Renderer_Uniform_Buffer > uniform_buffers; // Shader-bound declaration info. Read only.

	// OpenGL-specific:
	GLuint opengl_program;
};

struct Renderer_Pipeline {
	// Renderer_Shader

	// OpenGL-specific:
	GLuint opengl_pipeline;
};

enum Renderer_Framebuffer_Attachment_Point : u8 {
	RendererFramebufferAttachmentPoint_Color0 = 0,
	RendererFramebufferAttachmentPoint_Color1,
	RendererFramebufferAttachmentPoint_Color2,
	RendererFramebufferAttachmentPoint_Color3,
	RendererFramebufferAttachmentPoint_Color4,
	RendererFramebufferAttachmentPoint_Color5,
	RendererFramebufferAttachmentPoint_Color6,
	RendererFramebufferAttachmentPoint_Color7,
	RendererFramebufferAttachmentPoint_Color8,
	RendererFramebufferAttachmentPoint_Color9,
	RendererFramebufferAttachmentPoint_Color10,
	RendererFramebufferAttachmentPoint_Color11,
	RendererFramebufferAttachmentPoint_Color12,
	RendererFramebufferAttachmentPoint_Color13,
	RendererFramebufferAttachmentPoint_Color14,
	RendererFramebufferAttachmentPoint_Color15,

	RendererFramebufferAttachmentPoint_Depth,
	RendererFramebufferAttachmentPoint_Stencil,
	RendererFramebufferAttachmentPoint_DepthStencil,

	RendererFramebufferAttachmentPoint_None = U8_MAX
};

enum Renderer_Framebuffer_Attachment_Bit : u8 {
	RendererFramebufferAttachmentBit_IsRenderbuffer = ( 1 << 0 ),
	RendererFramebufferAttachmentBit_IsActive = ( 1 << 1 )
};
typedef u8 Renderer_Framebuffer_Attachment_Bits;

struct Renderer_Framebuffer_Attachment;
struct Renderer_Framebuffer {
	StringView_ASCII name;
	Array< Renderer_Framebuffer_Attachment > attachments;

	// OpenGL-specific:
	GLuint opengl_framebuffer;
};
typedef u8 Renderer_Framebuffer_ID;
constexpr Renderer_Framebuffer_ID INVALID_FRAMEBUFFER_ID = U8_MAX;

struct Renderer_Renderbuffer {
	StringView_ASCII name;
	Renderer_Framebuffer_Attachment_Point attachment_point;

	// OpenGL-specific:
	GLuint opengl_renderbuffer;
	// Stored in OpenGL (glGetRenderbufferParameter):
	// width
	// height
	// internal_format
	// samples
	// R G B A | D S component sizes
};
typedef u8 Renderer_Renderbuffer_ID;
constexpr Renderer_Renderbuffer_ID INVALID_RENDERBUFFER_ID = U8_MAX;

struct Renderer_Framebuffer_Attachment {
	Renderer_Framebuffer_Attachment_Point attachment_point;
	Renderer_Framebuffer_Attachment_Bits bits;
	union {
		Texture_ID _texture_id;
		Renderer_Renderbuffer_ID _renderbuffer_id;
	};
};

struct Renderer_Render_Command {
	Mesh_ID mesh_id;
	Material_ID material_id;
	Transform *transform;
};

enum Renderer_Output_Channel : u8 {
	RendererOutputChannel_FinalColor = 0,
	RendererOutputChannel_Position,
	RendererOutputChannel_Normal,
	RendererOutputChannel_DiffuseSpecular
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

u32
renderer_data_type_size( Renderer_Data_Type data_type );

GLenum
renderer_data_type_to_opengl_type( Renderer_Data_Type data_type );

Renderer_Renderbuffer_ID
renderer_create_renderbuffer(
	StringView_ASCII name,
	u32 width,
	u32 height,
	Renderer_Framebuffer_Attachment_Point attachment_point,
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
renderer_renderbuffer_attach_to_framebuffer( Renderer_Framebuffer_ID framebuffer_id, Renderer_Renderbuffer_ID renderbuffer_id, Renderer_Framebuffer_Attachment_Point attachment_point );

GLenum
renderer_framebuffer_attachment_point_to_opengl( Renderer_Framebuffer_Attachment_Point attachment_point );

bool
renderer_is_framebuffer_complete( Renderer_Framebuffer_ID framebuffer_id );

void
renderer_set_active_framebuffer_color_attachment_points( Renderer_Framebuffer_ID framebuffer_id, ArrayView< Renderer_Framebuffer_Attachment_Point > attachment_points );

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

Renderer_Framebuffer_Attachment_Point
renderer_texture_format_to_framebuffer_attachment_point( Texture_Format format );

bool
renderer_framebuffer_color_attachment_point_is_valid( Renderer_Framebuffer_Attachment_Point color_attachment_point );

bool
renderer_texture_attach_to_framebuffer( Texture_ID texture_id, Renderer_Framebuffer_ID framebuffer_id, Renderer_Framebuffer_Attachment_Point attachment_point );

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
renderer_uniform_buffer_create( StringView_ASCII name, u32 size, Renderer_GL_Buffer_Storage_Bits storage_bits, u32 binding = INVALID_UNIFORM_BUFFER_BINDING );

void
renderer_uniform_buffer_set_binding( Renderer_Uniform_Buffer *uniform_buffer, u32 binding );

void *
renderer_uniform_buffer_memory_map( Renderer_Uniform_Buffer *uniform_buffer, Renderer_GL_Map_Access_Bits access_bits, u32 size = 0, u32 offset = 0 );

bool
renderer_uniform_buffer_memory_unmap( Renderer_Uniform_Buffer *uniform_buffer, Renderer_GL_Map_Access_Bits access_bits, u32 size, u32 offset );

u32
renderer_uniform_buffer_write( Renderer_Uniform_Buffer *uniform_buffer, ArrayView< u8 > write_data, u32 size = 0, u32 offset = 0 );

u32
renderer_uniform_buffer_read( Renderer_Uniform_Buffer *uniform_buffer, Array< u8 > *out_read_data, u32 size = 0, u32 offset = 0 );

bool
renderer_uniform_buffer_destroy( Renderer_Uniform_Buffer *uniform_buffer );

void
renderer_set_camera_position_pointer( Vector3_f32 *camera_position );

void
renderer_set_ambient_light_color( Vector3_f32 ambient_light );

StringView_ASCII
renderer_framebuffer_attachment_point_name( Renderer_Framebuffer_Attachment_Point attachment_point );

f32
renderer_frame_time_delta();

void
renderer_set_output_channel( Renderer_Output_Channel channel );

#endif /* QLIGHT_RENDERER_H */
