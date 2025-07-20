#include "texture.h"
#include "../libs/stb/stb_image.h"

constexpr u64 TEXTURES_INITIAL_CAPACITY = 16;

struct {
	Array< Texture > textures;

	u32 created;
	u32 loaded;
	u32 destroyed;
	u32 searches;
	u32 instances_obtained;
} g_textures;

bool textures_init() {
	if ( g_textures.textures.data )
		return false;

	g_textures.textures = array_new< Texture >( sys_allocator, TEXTURES_INITIAL_CAPACITY );
	g_textures.created = 0;
	g_textures.loaded = 0;
	g_textures.destroyed = 0;
	g_textures.searches = 0;
	g_textures.instances_obtained = 0;

	stbi_set_flip_vertically_on_load( true );
	return true;
}

void textures_shutdown() {
	if ( !g_textures.textures.data )
		return;

	array_free( &g_textures.textures );
}

Texture_ID texture_create(
	StringView_ASCII name,
	u16 width,
	u16 height,
	Texture_Format format,
	GLint opengl_internal_format,
	GLenum opengl_pixel_type
) {
	Texture texture = {
		.name = name,
		.file_path = NULL,
		.width = width,
		.height = height,
		.format = format,
		.opengl_internal_format = opengl_internal_format,
		.opengl_pixel_type = opengl_pixel_type,
		.bytes = ArrayView< u8 > { .size = 0, .data = NULL }
		// .opengl_id
	};

	u32 texture_id = array_add( &g_textures.textures, texture );

	g_textures.created += 1;
	return texture_id;
}

// @CopyPaste from renderer. Currently Texture module is coupled with OpenGL...
static GLenum _texture_format_to_opengl( Texture_Format format ) {
	switch ( format ) {
		case TextureFormat_Red:           return GL_RED;
		case TextureFormat_RG:            return GL_RG;
		case TextureFormat_RGB:           return GL_RGB;
		case TextureFormat_RGBA:          return GL_RGBA;

		case TextureFormat_BGR:           return GL_BGR;
		case TextureFormat_BGRA:          return GL_BGRA;

		case TextureFormat_Depth:         return GL_DEPTH_COMPONENT;
		case TextureFormat_Stencil:       return GL_STENCIL_INDEX;
		case TextureFormat_DepthStencil:  return GL_DEPTH_STENCIL;

		case TextureFormat_Unknown:
		default:                          return GL_INVALID_ENUM;
	}
}

Texture_ID texture_load_from_file( StringView_ASCII name, StringView_ASCII file_path ) {
	Assert( name.size > 0 );
	Assert( file_path.size > 0 );

	Texture_Format expected_format = TextureFormat_RGB;
	int desired_channels = texture_format_channels( expected_format );

	int width;
	int height;
	int color_channels;
	ArrayView< u8 > texture_data;

	// @TODO: Use `stbi_load_from_memory` through own file system.
	texture_data.data = stbi_load( file_path.data, &width, &height, &color_channels, desired_channels );
	Assert( texture_data.data );
	if ( !texture_data.data )
		return INVALID_TEXTURE_ID;

	texture_data.size = width * height * color_channels * 1 /* GL_UNSIGNED_BYTE */;

	GLint opengl_internal_format = _texture_format_to_opengl( expected_format );
	Texture texture = {
		.name = name,
		.file_path = file_path,
		.width = static_cast< u16 >( width ),
		.height = static_cast< u16 >( height ),
		.format = expected_format,
		.opengl_internal_format = opengl_internal_format,
		.opengl_pixel_type = GL_UNSIGNED_BYTE,
		.bytes = texture_data
		// .opengl_id - generated on upload
	};

	Texture_ID texture_id = array_add( &g_textures.textures, texture );

	g_textures.loaded += 1;
	return texture_id;
}

Texture_ID texture_load_from_memory( StringView_ASCII name, ArrayView< u8 > memory ) {
	Assert( false );
	return INVALID_TEXTURE_ID;
}

bool texture_destroy( Texture_ID texture_id ) {
	// @TODO:
	// if ( g_textures.textures.data[ texture_id ]
	// g_textures.destroyed += 1;
	// stbi_free
	return false;
}

Texture_ID texture_find( StringView_ASCII name ) {
	g_textures.searches += 1;

	Texture *texture;
	for ( u32 texture_idx = 0; texture_idx < g_textures.textures.size; texture_idx += 1 ) {
		texture = &g_textures.textures.data[ texture_idx ];
		if ( string_equals( texture->name, name ) )
			return texture_idx;
	}

	return false;
}

Texture * texture_instance( Texture_ID texture_id ) {
	g_textures.instances_obtained += 1;
	return &g_textures.textures.data[ texture_id ];
}

u8 texture_format_channels( Texture_Format format ) {
	switch ( format ) {
		case TextureFormat_Red:           return 1;  // R
		case TextureFormat_RG:            return 2;  // R G
		case TextureFormat_RGB:           return 3;  // R G B
		case TextureFormat_RGBA:          return 4;  // R G B A

		case TextureFormat_Depth:         return 1;  // D
		case TextureFormat_Stencil:       return 1;  // S
		case TextureFormat_DepthStencil:  return 2;  // D S

		case TextureFormat_Unknown:
		default:                          return 0;
	}
}
