#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "model.h"
#include "renderer.h"

#define QL_LOG_CHANNEL "Model"
#include "log.h"

struct {
	Array< Model > models;
	Array< Mesh > meshes;
} g_models;

bool models_init() {
	if ( g_models.models.data )
		return false;

	g_models.models = array_new< Model >( sys_allocator, 8 );
	g_models.meshes = array_new< Mesh >( sys_allocator, 16 );
	return true;
}

void models_shutdown() {
	if ( !g_models.models.data )
		return;

	array_free( &g_models.models );
	array_free( &g_models.meshes );
}

Model_ID model_find( StringView_ASCII name ) {
	ForIt( g_models.models.data, g_models.models.size ) {
		if ( string_equals( name, it.name ) )
			return it_index;
	}}

	return INVALID_MODEL_ID;
}

Model * model_instance( Model_ID model_id ) {
	if ( model_id >= g_models.models.size )
		return NULL;

	return &g_models.models.data[ model_id ];
}

static u32
vertex_attributes_size( ArrayView< Renderer_Vertex_Attribute > attributes ) {
	u32 size = 0;
	ForIt( attributes.data, attributes.size ) {
		u32 atr_size = renderer_data_type_size( it.data_type ) * it.elements;
		size += atr_size;
	}}

	return size;
}

static u32
add_appropriately_sized_mesh_indices( CArray *destination, CArrayView source ) {
	if ( destination->item_size == source.item_size )
		// u32 -> u32
		return carray_add_many( destination, source );
	else {
		// u32 -> u16
		// Just to make sure this is the case of downcasting u32 indices to u16.
		Assert( destination->item_size == sizeof( u16 ) );
		Assert( source.item_size == sizeof( unsigned int ) );
		u32 added = 0;
		for ( u32 source_idx = 0; source_idx < source.size * source.item_size; source_idx += source.item_size ) {
			u16 index_value = ( u16 )source.data[ source_idx ];
			carray_add( destination, &index_value );
			added += 1;
		}
		return added;
	}
}

Model_ID model_load_from_file( StringView_ASCII name, StringView_ASCII file_path ) {
	log_debug( "Loading '" StringViewFormat "' from '" StringViewFormat "'...",
		StringViewArgument( name ),
		StringViewArgument( file_path )
	);
	const aiScene *scene = aiImportFile( file_path.data, 0 );
	Assert( scene );

	Array< Renderer_Vertex_Attribute > attributes = array_new< Renderer_Vertex_Attribute >( sys_allocator, 3 );

	array_add( &attributes, Renderer_Vertex_Attribute {
		.name = "position",
		.index = 0,
		.elements = 3,
		.data_type = RendererDataType_f32,
		.normalize = false,
		.active = true
	} );

	array_add( &attributes, Renderer_Vertex_Attribute {
		.name = "normal",
		.index = 1,
		.elements = 3,
		.data_type = RendererDataType_f32,
		.normalize = false,
		.active = true
	} );

	array_add( &attributes, Renderer_Vertex_Attribute {
		.name = "texture_uv",
		.index = 2,
		.elements = 2,
		.data_type = RendererDataType_f32,
		.normalize = false,
		.active = true
	} );

	const aiNode *root = scene->mRootNode;
	const aiMesh *ai_mesh = scene->mMeshes[ 0 ];
	u32 vertex_size = vertex_attributes_size( array_view( &attributes ) );
	u32 indices_count = ai_mesh->mNumFaces * 3;
	constexpr u32 INDICES_U16_MAX = ( U16_MAX - 1 ) / 3;
	u32 index_type_size = ( indices_count <= INDICES_U16_MAX ) ? sizeof( u16 ) : sizeof( u32 );
	StringView_ASCII mesh_name = string_view( ai_mesh->mName.data, 0, ai_mesh->mName.length );
	Mesh mesh = {
		.name = string_new( sys_allocator, mesh_name ),
		.vertices = carray_new( sys_allocator, /* item_size */ vertex_size, /* count */ ai_mesh->mNumVertices ),
		.indices = carray_new( sys_allocator, /* item_size */ index_type_size, /* count */ indices_count ), // Meshes are assumed to be triangulated (3 vertices per face)
		.material_id = INVALID_MATERIAL_ID,
		.bits1 = 0,
		.vertex_attributes = attributes
		// .opengl_vao
		// .opengl_vbo
		// .opengl_ebo
	};

	// Combine vertex data from sparse arrays:
	// from [XYZ, XYZ, XYZ...], [N, N, N...], [UV, UV, UV...], ...
	// to   [XYZ, N, UV; XYZ, N, UV...]
	For( ai_mesh->mNumVertices ) {
		Vector3_f32 position = {
			.x = ai_mesh->mVertices[ it_index ].x,
			.y = ai_mesh->mVertices[ it_index ].y,
			.z = ai_mesh->mVertices[ it_index ].z
		};

		Vector3_f32 normal = {
			.x = ai_mesh->mNormals[ it_index ].x,
			.y = ai_mesh->mNormals[ it_index ].y,
			.z = ai_mesh->mNormals[ it_index ].z
		};

		Vector2_f32 texture_uv = {
			.x = ai_mesh->mTextureCoords[ 0 ][ it_index ].x,
			.y = ai_mesh->mTextureCoords[ 0 ][ it_index ].y
			// .z = ai_mesh->mTextureCoords[ 0 ]->z
		};

		Vertex_3D vertex = {
			.position = position,
			.normal = normal,
			.texture_uv = texture_uv
		};

		carray_add( &mesh.vertices, &vertex );
	}

	ForIt( ai_mesh->mFaces, ai_mesh->mNumFaces ) {
		AssertMessage( it.mNumIndices == 3, "Non-triangle face in a mesh" );

		CArrayView face_indices = CArrayView {
			.size = it.mNumIndices,
			.item_size = sizeof( unsigned int ), // a.k.a. u32 - assimp index type's size
			.data = ( u8 * )it.mIndices
		};

		add_appropriately_sized_mesh_indices( &mesh.indices, face_indices );
	}}

	Model model = {
		.name = name,
		.transform = transform_identity(),
		.meshes = array_new< Mesh_ID >( sys_allocator, 1 )
	};

	Mesh_ID mesh_id = mesh_store( &mesh );
	array_add( &model.meshes, mesh_id );

	u32 model_idx = array_add( &g_models.models, model );

	aiReleaseImport( scene );
	log_info( "Loaded '" StringViewFormat "' (#%u, %u vertices, %u indices).",
		StringViewArgument( name ),
		model_idx,
		mesh.vertices.size,
		mesh.indices.size
	);
	return model_idx;
}

Mesh_ID mesh_store( Mesh *mesh ) {
	u32 mesh_idx = array_add( &g_models.meshes, *mesh );
	Mesh_ID mesh_id = mesh_idx;
	return mesh_id;
}

Mesh_ID mesh_find( StringView_ASCII name ) {
	ForIt( g_models.meshes.data, g_models.meshes.size ) {
		if ( string_equals( name, string_view( &it.name ) ) )
			return it_index;
	}}

	return INVALID_MESH_ID;
}

Mesh * mesh_instance( Mesh_ID mesh_id ) {
	if ( mesh_id >= g_models.meshes.size )
		return NULL;

	return &g_models.meshes.data[ mesh_id ];
}

u32 mesh_vertex_attributes_size( Mesh *mesh ) {
	ArrayView< Renderer_Vertex_Attribute > attributes = array_view( &mesh->vertex_attributes );
	u32 vertex_size = vertex_attributes_size( attributes );
	return vertex_size;
}

bool mesh_is_dynamic( Mesh *mesh ) {
	return ( mesh->bits1 & MeshBit_Dynamic );
}

bool mesh_is_dirty( Mesh *mesh ) {
	return ( mesh->bits1 & MeshBit_Dirty );
}

bool mesh_is_no_draw( Mesh *mesh ) {
	return ( mesh->bits1 & MeshBit_NoDraw );
}
