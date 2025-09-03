// We don't actually need to write code here,
// because all the code is already written in header file.
// All we need to do is just define STB_IMAGE_IMPLEMENTATION.
#include "../../src/common.h"

Allocator *stbi_allocator = sys_allocator;

#define STBI_MALLOC( size ) \
	Allocate( stbi_allocator, size, u8 )

// This does not actually get used, it is a fallback if sized version is not defined.
// #define STBI_REALLOC( pointer, new_size ) \
//	Reallocate( sys_allocator, pointer, 0, new_size, u8 )

#define STBI_FREE( pointer ) \
	Deallocate( stbi_allocator, pointer )

#define STBI_REALLOC_SIZED( pointer, old_size, new_size ) \
	Reallocate( stbi_allocator, pointer, old_size, new_size, u8 )

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"