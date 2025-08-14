#include "material.h"
#include "renderer.h"

#define QL_LOG_CHANNEL "Material"
#include "log.h"

constexpr u64 MATERIALS_INITIAL_CAPACITY = 64;

struct {
	Array< Material > materials;

	u64 created;
	u64 destroyed;
	u64 searches;
	u64 instances_obtained;
} g_materials;

bool materials_init() {
	if ( g_materials.materials.data )
		return false;

	g_materials.materials = array_new< Material >( sys_allocator, MATERIALS_INITIAL_CAPACITY );
	g_materials.created = 0;
	g_materials.destroyed = 0;
	g_materials.searches = 0;
	g_materials.instances_obtained = 0;

	return true;
}

void materials_shutdown() {
	if ( !g_materials.materials.data )
		return;

	array_free( &g_materials.materials );
}

Material_ID material_create(
	StringView_ASCII name,
	Renderer_Shader_Program *shader_program,
	Texture_ID diffuse,
	Texture_ID normal_map,
	Texture_ID specular_map,
	f32 shininess_exponent
) {
	Assert( name.size > 0 );
	Assert( shader_program );
	Assert( shininess_exponent >= 0.0f );

	Material material = {
		.name = name,
		.shader_program = shader_program,
		.diffuse = diffuse,
		.normal_map = normal_map,
		.specular_map = specular_map,
		.shininess_exponent = shininess_exponent
	};

	Material_ID material_id = array_add( &g_materials.materials, material );
	g_materials.created += 1;
	log_debug(
		"Created '" StringViewFormat "' (#%u, '" StringViewFormat "', diffuse: #%u, normal: #%u, specular: #%u, shininess: %.1f).",
		StringViewArgument( material.name ),
		material_id,
		StringViewArgument( material.shader_program->name ),
		material.diffuse,
		material.normal_map,
		material.specular_map,
		material.shininess_exponent
	);
	return material_id;
}

bool material_destroy( Material_ID material_id ) {
	// @TODO:
	// ...
	// g_amterials.destroyed += 1;
	return false;
}

Material_ID material_find( StringView_ASCII name ) {
	g_materials.searches += 1;

	ForIt( g_materials.materials.data, g_materials.materials.size ) {
		if ( string_equals( name, it.name ) )
			return it_index;
	}}

	return false;
}

Material * material_instance( Material_ID material_id ) {
	if ( material_id >= g_materials.materials.size )
		return NULL;

	g_materials.instances_obtained += 1;
	return &g_materials.materials.data[ material_id ];
}
