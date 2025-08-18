#include "map.h"
#include "renderer.h"

struct G_Maps {
	Array< Map > maps;
	Map *current;
	Map *changing_to;
} g_maps;

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

	Map map_empty = {
		.name = "empty",
		.title = "",
		.description = "",
		.file_path = "",
		// .models = array_new< Model_ID >( sys_allocator, 1 ),
		// .entities = array_new< Entity_ID >( sys_allocator, 1 ),
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
		// .entities = array_new< Entity_ID >( sys_allocator, 16 ),
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
