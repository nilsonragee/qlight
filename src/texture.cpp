#include "texture.h"
#include "../libs/stb/stb_image.h"

#define QL_LOG_CHANNEL "Texture"
#include "log.h"

constexpr u64 TEXTURES_INITIAL_CAPACITY = 16;

struct G_Texture {
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

ArrayView< Texture > textures_get_storage_view() {
	return array_view( &g_textures.textures );
}

Texture_ID texture_create(
	StringView_ASCII name,
	Vector2_u16 dimensions,
	Texture_Channels channels,
	ArrayView< u8 > bytes,
	Allocator *allocator
) {
	Texture texture = {
		.name = name,
		.file_path = NULL,
		.original_dimensions = dimensions,
		.dimensions = dimensions,
		.origin = { 0, 0 },
		.mipmap_levels = 0,
		.channels = channels,
		.opengl_storage_format = 0,
		.opengl_pixel_type = 0,
		// .bytes
		// .opengl_id
	};

	if ( allocator ) {
		// Allocate memory for the texture data array.
		if ( bytes.size == 0 ) {
			log_warning(
				"Trying to allocate CPU-sided memory with  size=0  while creating Texture '" StringViewFormat "'. "
				"No allocation will be made. Consider calling with  allocator=NULL  if this is intentional.",
				StringViewArgument( name )
			);
		}
		// The allocation will be done only if `bytes` is a valid non-zero view.
		texture.bytes = array_new< u8 >( sys_allocator, bytes );
	} else {
		// Do not allocate memory, only store as a view.
		texture.bytes = Array< u8 > {
			.allocator = NULL,
			.size = bytes.size,
			.capacity = bytes.size,
			.data = bytes.data
		};
		if ( bytes.data == NULL && bytes.size == 0 ) {
			u32 channels_count = texture_channels_count( channels );
			texture.bytes.size = dimensions.width * dimensions.height * channels_count * sizeof( u8 );
		}
	}

	u32 texture_id = array_add( &g_textures.textures, texture );

	g_textures.created += 1;
	log_info( "Created '" StringViewFormat "' (#%u, %hux%hu, " StringViewFormat ", %u bytes).",
		StringViewArgument( texture.name ),
		texture_id,
		dimensions.width,
		dimensions.height,
		StringViewArgument( texture_channels_name( texture.channels ) ),
		texture.bytes.size
	);
	return texture_id;
}

// @CopyPaste from renderer. Currently Texture module is coupled with OpenGL...
static GLenum _texture_channels_to_opengl( Texture_Channels channels ) {
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

Texture_ID texture_load_from_file(
	StringView_ASCII name,
	StringView_ASCII file_path,
	Texture_Channels desired_channels
) {
	log_debug( "Loading '" StringViewFormat "' from '" StringViewFormat "'...",
		StringViewArgument( name ),
		StringViewArgument( file_path )
	);
	Assert( name.size > 0 );
	Assert( file_path.size > 0 );

	int desired_channels_count = texture_channels_count( desired_channels );

	int width;
	int height;
	int color_channels;
	Array< u8 > texture_data;
	texture_data.allocator = stbi_allocator;

	// @TODO: Use `stbi_load_from_memory` through own file system.
	texture_data.data = stbi_load( file_path.data, &width, &height, &color_channels, desired_channels_count );
	Assert( texture_data.data );
	if ( !texture_data.data )
		return INVALID_TEXTURE_ID;

	texture_data.size = width * height * color_channels * sizeof( u8 ) /* GL_UNSIGNED_BYTE */;
	texture_data.capacity = texture_data.size;

	Assert( desired_channels_count == color_channels );
	Vector2_u16 dimensions = { ( u16 )width, ( u16 )height };
	Texture texture = {
		.name = name,
		.file_path = file_path,
		.original_dimensions = dimensions,
		.dimensions = dimensions,
		.origin = { 0, 0 },
		.mipmap_levels = 0,
		.channels = desired_channels,
		.opengl_storage_format = 0,
		.opengl_pixel_type = 0,
		.bytes = texture_data
		// .opengl_id - generated on upload
	};

	Texture_ID texture_id = array_add( &g_textures.textures, texture );

	g_textures.loaded += 1;
	log_info( "Loaded '" StringViewFormat "' (#%u, %hux%hu, " StringViewFormat ", %u bytes).",
		StringViewArgument( texture.name ),
		texture_id,
		dimensions.width,
		dimensions.height,
		StringViewArgument( texture_channels_name( texture.channels ) ),
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

	ForIt( g_textures.textures.data, g_textures.textures.size ) {
		if ( string_equals( name, it.name ) )
			return it_index;
	}}

	return INVALID_TEXTURE_ID;
}

Texture * texture_instance( Texture_ID texture_id ) {
	g_textures.instances_obtained += 1;
	return &g_textures.textures.data[ texture_id ];
}

u8 texture_channels_count( Texture_Channels channels ) {
	switch ( channels ) {
		case TextureChannels_Red:           return 1;  // R
		case TextureChannels_RG:            return 2;  // R G
		case TextureChannels_RGB:           return 3;  // R G B
		case TextureChannels_RGBA:          return 4;  // R G B A

		case TextureChannels_Depth:         return 1;  // D
		case TextureChannels_Stencil:       return 1;  // S
		case TextureChannels_DepthStencil:  return 2;  // D S

		case TextureChannels_None:
		default:                          return 0;
	}
}

StringView_ASCII texture_channels_name( Texture_Channels channels ) {
	switch ( channels ) {
		case TextureChannels_Red: return "Red";
		case TextureChannels_RG: return "RG";
		case TextureChannels_RGB: return "RGB";
		case TextureChannels_RGBA: return "RGBA";

		case TextureChannels_BGR: return "BGR";
		case TextureChannels_BGRA: return "BGRA";

		case TextureChannels_Depth: return "Depth";
		case TextureChannels_Stencil: return "Stencil";
		case TextureChannels_DepthStencil: return "DepthStencil";

		case TextureChannels_None:
		default: return "None";
	}
}
