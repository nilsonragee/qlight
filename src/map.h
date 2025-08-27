#ifndef QLIGHT_MAP
#define QLIGHT_MAP

#include "common.h"
#include "string.h"
#include "carray.h"
#include "model.h"
#include "entity.h"
#include "entity_table.h"
#include "renderer.h"

// std140 - 16-byte alignment required
struct Uniform_Buffer_Struct_Light {
	Vector4_f32 position; // w=0: directional, w=1: positional
	Vector4_f32 color;    // rgb: color, a: intensity
	f32 shininess_exponent;
	f32 _padding0[ 3 ]; // Align to 16 bytes.
};

#define MAX_LIGHT_SOURCES 32

// std140 - 16-byte alignment required
struct Uniform_Buffer_Lights {
	Uniform_Buffer_Struct_Light lights[ MAX_LIGHT_SOURCES ];
	u32 lights_count;
	f32 _padding0[ 3 ]; // Align to 16 bytes.
};

struct Lights_Manager {
	Renderer_Uniform_Buffer *uniform_buffer;
	Renderer_GL_Buffer_Mapping mapping;
	u32 current_slot;
	u32 prev_lights_count;
	u32 lights_count;
	Array< u16 > empty_slots;
	Array< Entity_ID > light_entities;
};

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

	CArray entity_storages[ EntityType_COUNT ];
	Entity_Lookup_Table entity_table;
	Map_State state;
	// Array< Entity > entities;
	// Array< Player > players;

	f32 time;
};

bool maps_init();
void maps_shutdown();

void maps_update_lights_manager();

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

Entity_ID map_entity_add( Map *map, Entity *entity );
bool map_entity_remove( Map *map, Entity_ID entity_id );

CArrayView map_stored_entities_of_type( Map *map, Entity_Type type );

void lights_manager_init( Map *map, Renderer_Uniform_Buffer *uniform_buffer_lights );
void lights_manager_destroy( Map *map );

#endif /* QLIGHT_MAP */
