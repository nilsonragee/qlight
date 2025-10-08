#ifndef QLIGHT_TEXTURE_H
#define QLIGHT_TEXTURE_H

#include "../libs/GLEW/glew.h" // OpenGL types

#include "common.h"
#include "string.h"

// maybe TextureComponents?
enum Texture_Channels : u16 {
	TextureChannels_None = 0,

	TextureChannels_Red,
	TextureChannels_RG,
	TextureChannels_RGB,
	TextureChannels_RGBA,

	TextureChannels_BGR,
	TextureChannels_BGRA,

	TextureChannels_Depth,
	TextureChannels_Stencil,  // OpenGL 4.4+
	TextureChannels_DepthStencil
};

struct Texture {
	StringView_ASCII name;
	StringView_ASCII file_path;

	// Texture resolution as it was loaded or created, before its upload to renderer.
	Vector2_u16 original_dimensions;

	// A rectangle defining a texture region that will be
	//  stored when uploaded to the renderer (GPU).
	Vector2_u16 dimensions;
	Vector2_u16 origin;

	u8 mipmap_levels;
	Texture_Channels channels;
	GLint opengl_storage_format;  // Sized internal storage format, for example: `GL_RGBA8`.
	GLenum opengl_pixel_type;  // Pixel data type, for example: `GL_UNSIGNED_BYTE`.
	Array< u8 > bytes;

	// OpenGL-specific:
	GLuint opengl_id;
};

typedef u16 Texture_ID;
constexpr u16 INVALID_TEXTURE_ID = U16_MAX;

bool textures_init();
void textures_shutdown();
ArrayView< Texture > textures_get_storage_view();

Texture_ID texture_create(
	StringView_ASCII name,
	Vector2_u16 dimensions,
	Texture_Channels channels,
	ArrayView< u8 > bytes,
	Allocator *allocator
);

Texture_ID texture_load_from_file( StringView_ASCII name, StringView_ASCII file_path, Texture_Channels desired_channels );
Texture_ID texture_load_from_memory( StringView_ASCII name, ArrayView< u8 > memory );
bool texture_destroy( Texture_ID texture_id );

Texture_ID texture_find( StringView_ASCII name );
Texture * texture_instance( Texture_ID texture_id );

// @TODO: Decouple from OpenGL
// static GLenum _texture_channels_to_opengl( Texture_Channels channels );
u8 texture_channels_count( Texture_Channels channels );
StringView_ASCII texture_channels_name( Texture_Channels channels );

#endif /* QLIGHT_TEXTURE_H */
