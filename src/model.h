#ifndef QLIGHT_MODEL_H
#define QLIGHT_MODEL_H

#include "common.h"
#include "string.h"
#include "carray.h"

#include "material.h"
#include "texture.h"
#include "transform.h"

// #include "renderer.h"
struct Vertex_3D;
struct Renderer_Vertex_Attribute;

/*
struct Mesh_Data {
	Array< Vertex_3D > vertices;
	Array< u32 > indices;
};
*/

enum EMesh_Bits : u16 {
	MeshBit_Dynamic  = ( 1 << 0 ),  // Dynamic geometry.
	MeshBit_Dirty    = ( 1 << 1 ),  // Needs `normal_matrix` update.
	MeshBit_NoDraw   = ( 1 << 2 )   // Do not draw.
};
typedef u16 Mesh_Bits;

typedef u16 Model_ID;
constexpr Model_ID INVALID_MODEL_ID = U16_MAX;

typedef u16 Mesh_ID;
constexpr Mesh_ID INVALID_MESH_ID = U16_MAX;

struct Mesh {
	String_ASCII name;

	/*
		CPU mesh data.

		Stores its vertices, indices, and textures.

		If a mesh is static -- does not need data updates -- and is uploaded to GPU,
		then its CPU data can be dealloced.  In this case, the pointer is NULL.

		If a mesh is dynamic -- needs data updates (skeletal animation, particles, etc.)
		-- then its CPU data is kept allocated and updated.  In this case, the pointer is not NULL.
	*/
	// Mesh_Data *data;
	//Array< Vertex_3D > vertices;
	CArray vertices;
	// Array< u32 > indices; // TODO: use smaller type depending on amount
	CArray indices;

	Material_ID material_id;
	Mesh_Bits bits1; // Internal flags

	/*
		Vertex attributes.

		Stores mesh vertex data layout.

		@TODO: For now, it is directly dependant on Mesh_Data structure layout.
		Make it so the layout can be configured.
	*/
	Array< Renderer_Vertex_Attribute > vertex_attributes;

	// OpenGL-specific:
	/*
		Vertex Array Object

		Stores state of vertex attribute layout configuration (enabled vertex attribute pointers),
		and implicitly bound VBO and EBO.
	*/
	GLuint opengl_vao;

	/*
		Vertex Buffer Object

		A handle to VRAM buffer (GPU memory) which contains vertex data.
		Gets bound to active VAO after calling `glVertexAttribPointer`.
	*/
	GLuint opengl_vbo;

	/*
		Element Buffer Object

		A handle to VRAM buffer (GPU memory) which contains vertex index data.
		Gets bound to active VAO after calling `glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo )`.
	*/
	GLuint opengl_ebo;
};

struct Model {
	StringView_ASCII name;
	Transform transform;
	Array< Mesh_ID > meshes;
};

bool models_init();
void models_shutdown();

// Mesh mesh_load( StringView_ASCII file_path );
Model_ID model_load_from_file( StringView_ASCII name, StringView_ASCII file_path );
Model_ID model_find( StringView_ASCII name );
Model * model_instance( Model_ID model_id );

Mesh_ID mesh_store( Mesh *mesh );
Mesh_ID mesh_find( StringView_ASCII name );
Mesh * mesh_instance( Mesh_ID mesh_id );

u32 mesh_vertex_attributes_size( Mesh *mesh, u8 binding );

bool mesh_is_dynamic( Mesh *mesh );
bool mesh_is_dirty( Mesh *mesh );
bool mesh_is_no_draw( Mesh *mesh );

#endif /* QLIGHT_MODEL_H */