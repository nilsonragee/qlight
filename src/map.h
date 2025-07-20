#ifndef QLIGHT_MAP
#define QLIGHT_MAP

#include "common.h"
#include "string.h"
#include "carray.h"
#include "model.h"
#include "entity.h"

enum Map_State : u32 {
	MapState_NotLoaded = 0,
	MapState_Loading,
	MapState_Loaded,

	MapState_PreparePlay,
	MapState_Play,
	MapState_Idle, // No players on map
	MapState_Paused, // Time stop - maybe separate bool?

	MapState_Ending,
	MapState_Ended,

	MapState_Unloading,
	MapState_Unloaded
};

struct Map {
	StringView_ASCII name; // Technical name
	StringView_ASCII title; // Display name
	StringView_ASCII description;
	StringView_ASCII file_path;

	// CArray entity_storages[ EntityType_COUNT ];
	Array< Entity_ID > entities;
	Map_State state;
	// Array< Entity > entities;
	// Array< Player > players;

	f32 time;
};

bool maps_init();
void maps_shutdown();

void maps_load();

Map * map_load_from_file( StringView_ASCII name, StringView_ASCII file_path );
Map * map_load_by_name( StringView_ASCII name );
bool map_change( StringView_ASCII name );
void map_reload( Map *map );
void map_unload( Map *map );
void map_pause( Map *map, bool pause );
void map_start( Map *map );
void map_end( Map *map );
// void map_save( Map *map );

void map_draw( Map *map );

Map * map_current();
Map * map_changing_to();

#endif /* QLIGHT_MAP */
