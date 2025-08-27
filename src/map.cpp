#include "map.h"
#include "renderer.h"

struct G_Maps {
	Array< Map > maps;
	Map *current;
	Map *changing_to;
	Lights_Manager lights_manager;
	bool lights_manager_needs_update;
} g_maps;

static void
lights_manager_init() {
	Renderer_GL_Buffer_Storage_Bits storage_bits = renderer_gl_buffer_storage_bits(
		RendererGLBufferStorageBit_Write |
		RendererGLBufferStorageBit_Persistent |
		RendererGLBufferStorageBit_Coherent
	);

	Renderer_Uniform_Buffer *uniform_buffer_lights = renderer_uniform_buffer_create(
		/*         name */ "Lights",
		/*         size */ sizeof( Uniform_Buffer_Lights ),
		/* storage_bits */ storage_bits,
		/*      binding */ 0
	);

	Renderer_GL_Map_Access_Bits access_bits = renderer_gl_buffer_storage_bits_to_map_access_bits( storage_bits );
	u32 size = sizeof( Uniform_Buffer_Lights );
	u32 offset = 0;

	void *uniform_data = renderer_uniform_buffer_memory_map(
		/* uniform_buffer */ uniform_buffer_lights,
		/*    access_bits */ access_bits,
		/*           size */ size,
		/*         offset */ offset
	);

	Renderer_GL_Buffer_Mapping mapping = {
		.view = array_view< u8 >( ( u8 * )uniform_data, size, 0, size ),
		.offset = offset,
		.access_bits = access_bits,
		.opengl_buffer_id = uniform_buffer_lights->opengl_ubo
	};

	g_maps.lights_manager = {
		.uniform_buffer = uniform_buffer_lights,
		.mapping = mapping,
		.current_slot = 0,
		.prev_lights_count = 0,
		.lights_count = 0,
		.empty_slots = array_new< u16 >( sys_allocator, 16 ),
		.light_entities = array_new< Entity_ID >( sys_allocator, MAX_LIGHT_SOURCES )
	};
}

static void
lights_manager_destroy() {
	Lights_Manager *lights = &g_maps.lights_manager;
	bool destroyed = renderer_uniform_buffer_destroy( lights->uniform_buffer );
	Assert( destroyed );

	array_free( &lights->empty_slots );
	array_free( &lights->light_entities );

	lights->uniform_buffer = NULL;
	lights->current_slot = 0;
	lights->prev_lights_count = 0;
	lights->lights_count = 0;
}

static void
lights_manager_update() {
	Lights_Manager *lights = &g_maps.lights_manager;
	Map *map = g_maps.current;
	CArrayView c_directional_lights = map_stored_entities_of_type( map, EntityType_DirectionalLight );
	CArrayView c_point_lights = map_stored_entities_of_type( map, EntityType_PointLight );
	CArrayView c_spot_lights = map_stored_entities_of_type( map, EntityType_SpotLight );

	ArrayView< Entity_Directional_Light > directional_lights = array_view< Entity_Directional_Light >(
		/* data */ ( Entity_Directional_Light * )c_directional_lights.data,
		/* size */ c_directional_lights.size
	);
	ArrayView< Entity_Point_Light > point_lights = array_view< Entity_Point_Light >(
		/* data */ ( Entity_Point_Light * )c_point_lights.data,
		/* size */ c_point_lights.size
	);
	ArrayView< Entity_Spot_Light > spot_lights = array_view< Entity_Spot_Light >(
		/* data */ ( Entity_Spot_Light * )c_spot_lights.data,
		/* size */ c_spot_lights.size
	);

	/*
		if ( it.type == EntityType_DirectionalLight ) {
			// what about Entity_Spot_Light?
			position.w = 0.0f; // positional -> directional
		}
	*/

	Uniform_Buffer_Lights *uniform_data = ( Uniform_Buffer_Lights * )lights->mapping.view.data;
	Uniform_Buffer_Struct_Light *data_lights = ( Uniform_Buffer_Struct_Light * )lights->mapping.view.data;
	ForIt( directional_lights.data, directional_lights.size ) {
		u32 light_idx = lights->current_slot + it_index;

		Vector4_f32 color = { it.color.r, it.color.g, it.color.b, it.intensity };
		Vector3_f32 *t_pos = &it.transform.position;
		Vector4_f32 position = { t_pos->x, t_pos->y, t_pos->z, 0.0f }; // directional -> .w = 0

		Uniform_Buffer_Struct_Light uniform_light = {
			.position = position,
			.color = color,
			.shininess_exponent = it.shininess_exponent,
			._padding0 = { 0 }
		};
		data_lights[ light_idx ] = uniform_light;
		lights->current_slot += 1;
		lights->lights_count += 1;
	}}

	ForIt( point_lights.data, point_lights.size ) {
		u32 light_idx = lights->current_slot + it_index;

		Vector4_f32 color = { it.color.r, it.color.g, it.color.b, it.intensity };
		Vector3_f32 *t_pos = &it.transform.position;
		Vector4_f32 position = { t_pos->x, t_pos->y, t_pos->z, 1.0f }; // positional -> .w = 1

		Uniform_Buffer_Struct_Light uniform_light = {
			.position = position,
			.color = color,
			.shininess_exponent = it.shininess_exponent,
			._padding0 = { 0 }
		};
		data_lights[ light_idx ] = uniform_light;
		lights->current_slot += 1;
		lights->lights_count += 1;
	}}

	ForIt( spot_lights.data, spot_lights.size ) {
		u32 light_idx = lights->current_slot + it_index;

		Vector4_f32 color = { it.color.r, it.color.g, it.color.b, it.intensity };
		Vector3_f32 *t_pos = &it.transform.position;
		Vector4_f32 position = { t_pos->x, t_pos->y, t_pos->z, 1.0f }; // positional -> .w = 1

		Uniform_Buffer_Struct_Light uniform_light = {
			.position = position,
			.color = color,
			.shininess_exponent = it.shininess_exponent,
			._padding0 = { 0 }
		};
		data_lights[ light_idx ] = uniform_light;
		lights->current_slot += 1;
		lights->lights_count += 1;
	}}

	u32 lights_count_offset = offsetof( Uniform_Buffer_Lights, lights_count );
	u32 *uniform_lights_count = ( u32 * )( ( u8 * )uniform_data + lights_count_offset );
	lights->prev_lights_count = *uniform_lights_count;
	*uniform_lights_count = lights->lights_count;

	g_maps.lights_manager_needs_update = false;
}

static void
entity_storages_free( Map *map ) {
	carray_free( &map->entity_storages[ EntityType_Player ] );
	carray_free( &map->entity_storages[ EntityType_Camera ] );
	carray_free( &map->entity_storages[ EntityType_StaticObject ] );
	carray_free( &map->entity_storages[ EntityType_DynamicObject ] );

	carray_free( &map->entity_storages[ EntityType_DirectionalLight ] );
	carray_free( &map->entity_storages[ EntityType_PointLight ] );
	carray_free( &map->entity_storages[ EntityType_SpotLight ] );
}

static void
entity_storages_init( Map *map ) {
	map->entity_storages[ EntityType_Player ] = carray_new( sys_allocator, sizeof( Entity_Player ), 2 );
	map->entity_storages[ EntityType_Camera ] = carray_new( sys_allocator, sizeof( Entity_Camera ), 2 );
	map->entity_storages[ EntityType_StaticObject ] = carray_new( sys_allocator, sizeof( Entity_Static_Object ), 16 );
	map->entity_storages[ EntityType_DynamicObject ] = carray_new( sys_allocator, sizeof( Entity_Dynamic_Object ), 8 );

	map->entity_storages[ EntityType_DirectionalLight ] = carray_new( sys_allocator, sizeof( Entity_Directional_Light ), 2 );
	map->entity_storages[ EntityType_PointLight ] = carray_new( sys_allocator, sizeof( Entity_Point_Light ), 8 );
	map->entity_storages[ EntityType_SpotLight ] = carray_new( sys_allocator, sizeof( Entity_Point_Light ), 4 );
}

bool maps_init() {
	if ( g_maps.maps.data )
		return false;

	g_maps.maps = array_new< Map >( sys_allocator, 2 );
	g_maps.current = NULL;
	g_maps.changing_to = NULL;
	lights_manager_init();

	Map map_empty = {
		.name = "empty",
		.title = "",
		.description = "",
		.file_path = "",
		// .entity_storages
		.state = MapState_Loaded
	};
	u32 map_empty_idx = array_add( &g_maps.maps, map_empty );
	Map *map = &g_maps.maps.data[ map_empty_idx ];
	entity_storages_init( map );
	entity_table_init( &map->entity_table, sys_allocator, 32 );

	Map map_test = {
		.name = "test",
		.title = "Test Map",
		.description = "",
		.file_path = "test.map",
		// .entity_storages
		.state = MapState_NotLoaded
	};
	u32 map_test_idx = array_add( &g_maps.maps, map_test );
	map = &g_maps.maps.data[ map_test_idx ];
	entity_storages_init( map );
	entity_table_init( &map->entity_table, sys_allocator, 32 );

	// This is so we do not dereference NULL pointer on first change.
	g_maps.current = &g_maps.maps.data[ map_empty_idx ];
	map_change( "empty" );
	return true;
}

void maps_shutdown() {
	if ( !g_maps.maps.data )
		return;

	ForIt( g_maps.maps.data, g_maps.maps.size ) {
		entity_storages_free( &it );
		entity_table_destroy( &it.entity_table );
	}}
	array_free( &g_maps.maps );
}

void maps_update_lights_manager() {
	if ( g_maps.lights_manager_needs_update )
		lights_manager_update();
}

Map *map_load_from_file( StringView_ASCII name, StringView_ASCII file_path ) {
	Assert( false );
	return NULL;
}

Map *map_load_by_name( StringView_ASCII name ) {
	ForIt( g_maps.maps.data, g_maps.maps.size ) {
		if ( string_equals( name, it.name ) ) {
			Assert( it.state == MapState_NotLoaded || it.state == MapState_Unloaded );
			it.state = MapState_Loading;

			// Loading logic
			// ...

			it.state = MapState_Loaded;

			// Post-load logic
			// ...

			return &it;
		}
	}}

	return NULL;
}

bool map_change( StringView_ASCII name ) {
	if ( string_equals( name, g_maps.current->name ) ) {
		map_reload( g_maps.current );
		return true;
	}

	ForIt( g_maps.maps.data, g_maps.maps.size ) {
		if ( string_equals( name, it.name ) ) {
			g_maps.changing_to = &it;
			if ( it.state != MapState_Loaded ) {
				map_load_by_name( name );
			}

			map_end( g_maps.current );
			map_start( g_maps.changing_to );
			map_unload( g_maps.current );

			g_maps.current = &it;
			g_maps.changing_to = NULL;
			return true;
		}
	}}

	return false;
}

void map_reload( Map *map ) {
	if ( !map )
		return;

	Assert( map->state == MapState_NotLoaded || map->state == MapState_Loaded || map->state == MapState_Ended || map->state == MapState_Unloaded );
	map_unload( map );
	map_load_by_name( map->name );
}

void map_unload( Map *map ) {
	if ( !map )
		return;

	Assert( map->state == MapState_Loaded || map->state == MapState_Ended );
	map->state = MapState_Unloading;

	// Unloading logic
	// ...

	map->state = MapState_Unloaded;
}

void map_pause( Map *map, bool pause ) {
	if ( !map )
		return;

	if ( pause ) {
		Assert( map->state == MapState_Play || map->state == MapState_Idle );
		map->state = MapState_Paused;
	} else {
		Assert( map->state == MapState_Paused );
		map->state = MapState_Play;
	}
}

void map_start( Map *map ) {
	if ( !map )
		return;

	Assert( map->state == MapState_Loaded );
	map->state = MapState_PreparePlay;

	// Prepare play logic
	// ...

	map->state = MapState_Play;
}

void map_end( Map *map ) {
	if ( !map )
		return;

	Assert( map->state == MapState_Play || map->state == MapState_Idle || map->state == MapState_Paused );
	map->state = MapState_Ending;

	// Ending logic
	// ...

	map->state = MapState_Ended;
}

void map_draw( Map *map ) {
	/*
	for ( u32 model_idx = 0; model_idx < map->models.size; model_idx += 1 ) {
		Model_ID model_id = map->models.data[ model_idx ];
		Model *model = model_instance( model_id );
		for ( u32 mesh_idx = 0; mesh_idx < model->meshes.size; mesh_idx += 1 ) {
			Mesh_ID mesh_id = model->meshes.data[ mesh_idx ];
			bool no_draw = mesh_is_no_draw( mesh_id );
			if ( !no_draw );
				renderer_queue_draw_mesh( mesh_id );
		}
	}
	*/
}

Map * map_current() {
	return g_maps.current;
}

Map * map_changing_to() {
	return g_maps.changing_to;
}

Entity_ID
map_entity_add( Map *map, Entity *entity ) {
	// 1. Store `Entity` in an entity storage of that `Entity_Type`.
	CArray *entity_storage = &map->entity_storages[ entity->type ];
	u32 storage_idx = carray_add( entity_storage, entity );
	Entity *storage_entity = ( Entity * )carray_at( entity_storage, storage_idx );
	Assert( memcmp( storage_entity, entity, sizeof( Entity ) ) == 0 );

	// 2. Add reference to that stored `Entity` to the map's `Entity_Lookup_Table`.
	Entity_ID entity_id = entity_table_add( &map->entity_table, storage_entity );

	switch ( entity->type ) {
		case EntityType_DirectionalLight:
		case EntityType_PointLight:
		case EntityType_SpotLight:
			g_maps.lights_manager_needs_update = true;
	}
	return entity_id;
}

bool
map_entity_remove( Map *map, Entity_ID entity_id ) {
	if ( entity_id == INVALID_ENTITY_ID )
		return false;

	// 1. Find the reference of `Entity` in `Entity_Lookup_Table`.
	// It is not removed right away with `entity_table_remove` because
	//   the entity has to be removed from the entity storage first.
	//   (right now it is 'removed' via clearing - setting data with zeroes)
	Entity_Lookup_Table *table = &map->entity_table;
	Entity *entity = entity_table_find( table, entity_id );
	if ( !entity )
		return false;

	// 2. Remove `Entity` from the entity storage of that `Entity_Type`.
	CArray *entity_storage = &map->entity_storages[ entity->type ];
	carray_remove_at_pointer( entity_storage, entity );

	bool removed = entity_table_remove( table, entity_id );
	return removed;
}

CArrayView
map_stored_entities_of_type( Map *map, Entity_Type type ) {
	CArray *entity_storage = &map->entity_storages[ type ];
	CArrayView view = carray_view( entity_storage );
	return view;
}
