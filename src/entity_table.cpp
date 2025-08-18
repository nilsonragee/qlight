#include "entity_table.h"

void
entity_table_init( Entity_Lookup_Table *table, Allocator *allocator, u32 initial_capacity ) {
	Assert( !table->ids.data );
	Assert( !table->entities.data );

	table->ids = array_new< Entity_ID >( allocator, initial_capacity );
	table->entities = array_new< Entity * >( allocator, initial_capacity );
	table->slots_occupied = 0;
	table->counter = 0;
}

void
entity_table_clear( Entity_Lookup_Table *table, bool zero_memory ) {
	array_clear( &table->ids, zero_memory );
	array_clear( &table->entities, zero_memory );
	table->slots_occupied = 0;
	// table->counter = 0; // Preserve counter so there are no ID clashes?
}

void
entity_table_destroy( Entity_Lookup_Table *table, bool zero_memory ) {
	array_free( &table->ids, zero_memory );
	array_free( &table->entities, zero_memory );
	table->slots_occupied = 0;
	table->counter = 0;
}

Entity *
entity_table_find( Entity_Lookup_Table *table, Entity_ID entity_id ) {
	Assert( entity_id != INVALID_ENTITY_ID );
	ForIt( table->ids.data, table->ids.size ) {
		if ( it == entity_id ) {
			Entity *entity = table->entities.data[ it_index ];
			return entity;
		}
	}}
	return NULL;
}

Entity_ID
entity_table_add( Entity_Lookup_Table *table, Entity *entity ) {
	Assert( entity );
	if ( !entity )
		return INVALID_ENTITY_ID;

	// Search for empty slot and insert if found one.
	ForIt( table->entities.data, table->entities.size ) {
		if ( it == NULL ) {
			it = entity;
			Entity_ID *id = &table->ids.data[ it_index ];
			*id = table->counter;

			table->counter += 1;
			table->slots_occupied += 1;
			return *id;
		}
	}}

	// No empty slots found, add a new one.
	Entity_ID id = table->counter;
	array_add( &table->ids, id );
	array_add( &table->entities, entity );
	table->counter += 1;
	return id;
}

bool
entity_table_remove( Entity_Lookup_Table *table, Entity_ID entity_id ) {
	Assert( entity_id != INVALID_ENTITY_ID );
	if ( entity_id == INVALID_ENTITY_ID )
		return false;

	// Search for the entity and empty the slot if found one.
	ForIt( table->ids.data, table->ids.size ) {
		if ( it == entity_id ) {
			Entity **entity = &table->entities.data[ it_index ];
			*entity = NULL;
			table->slots_occupied =- 1;
			return true;
		}
	}}

	// Entity has not been found.
	return false;
}
