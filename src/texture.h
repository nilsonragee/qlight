#ifndef QLIGHT_TEXTURE_H
#define QLIGHT_TEXTURE_H

#include "../libs/GLEW/glew.h" // OpenGL types

#include "common.h"
#include "string.h"

// maybe TextureComponents?
enum Texture_Format : u16 {
	TextureFormat_Unknown = 0,

	TextureFormat_Red,
	TextureFormat_RG,
	TextureFormat_RGB,
	TextureFormat_RGBA,

	TextureFormat_BGR,
	TextureFormat_BGRA,

	TextureFormat_Depth,
	TextureFormat_Stencil,  // OpenGL 4.4+
	TextureFormat_DepthStencil
};

struct Texture {
	StringView_ASCII name;
	StringView_ASCII file_path;
	u16 width;
	u16 height;
	// u8 color_channels;
	Texture_Format format;
	GLint opengl_internal_format;
	GLenum opengl_pixel_type;
	ArrayView< u8 > bytes;

	// OpenGL-specific:
	GLuint opengl_id;
};

typedef u16 Texture_ID;
constexpr u16 INVALID_TEXTURE_ID = U16_MAX;

bool textures_init();
void textures_shutdown();

Texture_ID texture_create(
	StringView_ASCII name,
	u16 width,
	u16 height,
	Texture_Format format,
	GLint opengl_internal_format,
	GLenum opengl_pixel_type
);

Texture_ID texture_load_from_file( StringView_ASCII name, StringView_ASCII file_path );
Texture_ID texture_load_from_memory( StringView_ASCII name, ArrayView< u8 > memory );
bool texture_destroy( Texture_ID texture_id );

Texture_ID texture_find( StringView_ASCII name );
Texture * texture_instance( Texture_ID texture_id );

// @TODO: Decouple from OpenGL
// static GLenum _texture_format_to_opengl( Texture_Format format );
u8 texture_format_channels( Texture_Format format );
StringView_ASCII texture_format_name( Texture_Format format );
StringView_ASCII opengl_internal_texture_format_name( GLint internal_format );

#endif /* QLIGHT_TEXTURE_H */
