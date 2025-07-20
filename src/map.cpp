#include "map.h"
#include "renderer.h"

struct {
	Array< Map > maps;
	Map *current;
	Map *changing_to;
} g_maps;

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
		.entities = array_new< Entity_ID >( sys_allocator, 1 ),
		.state = MapState_Loaded
	};
	u32 map_empty_idx = array_add( &g_maps.maps, map_empty );

	Map map_test = {
		.name = "test",
		.title = "Test Map",
		.description = "",
		.file_path = "test.map",
		.entities = array_new< Entity_ID >( sys_allocator, 16 ),
		.state = MapState_NotLoaded
	};
	/*
	for ( Entity_Type entity_type = 0; entity_type < EntityType_COUNT; entity_type += 1 ) {
		Array< Entity_ID > *array = &map_test.entities[ entity_type ];
		*array = array_new< Entity_ID >( sys_allocator, 8 );
	}
	*/
	u32 map_test_idx = array_add( &g_maps.maps, map_test );

	// This is so we do not dereference NULL pointer on first change.
	g_maps.current = &g_maps.maps.data[ map_empty_idx ];
	map_change( "empty" );
	return true;
}

void maps_shutdown() {
	if ( !g_maps.maps.data )
		return;

	array_free( &g_maps.maps );
}

Map *map_load_from_file( StringView_ASCII name, StringView_ASCII file_path ) {
	Assert( false );
	return NULL;
}

Map *map_load_by_name( StringView_ASCII name ) {
	for ( u32 map_idx = 0; map_idx < g_maps.maps.size; map_idx += 1 ) {
		Map *map = &g_maps.maps.data[ map_idx ];
		if ( string_equals( name, map->name ) ) {
			Assert( map->state == MapState_NotLoaded || map->state == MapState_Unloaded );
			map->state = MapState_Loading;

			// Loading logic
			// ...

			map->state = MapState_Loaded;

			// Post-load logic
			// ...

			return map;
		}
	}

	return NULL;
}

bool map_change( StringView_ASCII name ) {
	if ( string_equals( name, g_maps.current->name ) ) {
		map_reload( g_maps.current );
		return true;
	}

	for ( u32 map_idx = 0; map_idx < g_maps.maps.size; map_idx += 1 ) {
		Map *map = &g_maps.maps.data[ map_idx ];
		if ( string_equals( name, map->name ) ) {
			g_maps.changing_to = map;
			if ( map->state != MapState_Loaded ) {
				map_load_by_name( name );
			}

			map_end( g_maps.current );
			map_start( g_maps.changing_to );
			map_unload( g_maps.current );

			g_maps.current = map;
			g_maps.changing_to = NULL;
			return true;
		}
	}

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
