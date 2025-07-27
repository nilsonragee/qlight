#include "texture.h"
#include "../libs/stb/stb_image.h"

#define QL_LOG_CHANNEL "Texture"
#include "log.h"

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
	log_info( "Created '" StringViewFormat "' (#%u, %ux%u, " StringViewFormat ", " StringViewFormat ", %u bytes).",
		StringViewArgument( texture.name ),
		texture_id,
		texture.width,
		texture.height,
		StringViewArgument( texture_format_name( texture.format ) ),
		StringViewArgument( opengl_internal_texture_format_name( texture.opengl_internal_format ) ),
		texture.bytes.size
	);
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
	log_debug( "Loading '" StringViewFormat "' from '" StringViewFormat "'...",
		StringViewArgument( name ),
		StringViewArgument( file_path )
	);
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

	texture_data.size = width * height * color_channels * sizeof( u8 ) /* GL_UNSIGNED_BYTE */;

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
	log_info( "Loaded '" StringViewFormat "' (#%u, %ux%u, " StringViewFormat ", " StringViewFormat ", %u bytes).",
		StringViewArgument( texture.name ),
		texture_id,
		texture.width,
		texture.height,
		StringViewArgument( texture_format_name( texture.format ) ),
		StringViewArgument( opengl_internal_texture_format_name( texture.opengl_internal_format ) ),
		texture.bytes.size
	);
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

StringView_ASCII texture_format_name( Texture_Format format ) {
	switch ( format ) {
		case TextureFormat_Red: return "Red";
		case TextureFormat_RG: return "RG";
		case TextureFormat_RGB: return "RGB";
		case TextureFormat_RGBA: return "RGBA";

		case TextureFormat_BGR: return "BGR";
		case TextureFormat_BGRA: return "BGRA";

		case TextureFormat_Depth: return "Depth";
		case TextureFormat_Stencil: return "Stencil";
		case TextureFormat_DepthStencil: return "DepthStencil";

		case TextureFormat_Unknown:
		default: return "Unknown";
	}
}

// TODO: Move out of Texture module
StringView_ASCII
opengl_internal_texture_format_name( GLint internal_format ) {
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
