#ifndef QLIGHT_ENTITY_TABLE
#define QLIGHT_ENTITY_TABLE

#include "entity.h"

struct Entity_Lookup_Table {
	Array< Entity_ID > ids;
	Array< Entity * > entities;
	u32 slots_occupied;
	Entity_ID counter;
};

     void entity_table_init( Entity_Lookup_Table *table, Allocator *allocator, u32 initial_capacity );
     void entity_table_clear( Entity_Lookup_Table *table, bool zero_memory = false );
     void entity_table_destroy( Entity_Lookup_Table *table, bool zero_memory = false );
 Entity * entity_table_find( Entity_Lookup_Table *table, Entity_ID entity_id );
Entity_ID entity_table_add( Entity_Lookup_Table *table, Entity *entity );
     bool entity_table_remove( Entity_Lookup_Table *table, Entity_ID entity_id );

#endif /* QLIGHT_ENTITY_TABLE */
