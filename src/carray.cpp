#include "carray.h"

#include <string.h>

CArray carray_new(Allocator *allocator, u32 item_size, u32 initial_capacity) {
	CArray array;
	array.allocator = allocator;
	array.size = 0;
	array.capacity = initial_capacity;
	array.data = (initial_capacity > 0) ? Allocate(allocator, array.capacity * item_size, u8) : NULL;
	// array.data = (initial_capacity > 0) ? TemplateAllocate(allocator, array.capacity, T) : NULL;
	array.item_size = item_size;
	return array;
}

// count = 0 -- means count to the end of the array's size.
CArrayView get_carray_view(CArray *array, u32 offset, u32 count) {
	return carray_view(array, offset, count);
}

// count = 0 -- means count to the end of the array's size.
CArrayView carray_view(CArray *array, u32 offset, u32 count) {
	AssertMessage(array, "CArrayView array pointer is NULL");
	AssertMessage(offset <= array->size, "CArrayView array offset is out of bounds");
	if (offset > array->size) {
		// If array offset is out of bounds, return everything zeroed.
		CArrayView view = { /* size */ 0, /* data */ NULL };
		return view;
	}

	u8 *view_data = array->data + offset;

	const u32 view_size_available = array->size - offset;
	const bool count_to_end = (count == 0);
	u32 view_size;
	if (count_to_end) {
		view_size = view_size_available;
	} else {
		AssertMessage(count <= view_size_available, "CArrayView size is out of bounds");
		// If view size is overflowing, clamp it to the end of array.
		view_size = (count <= view_size_available) ? count : view_size_available;
	}

	CArrayView view = { /* size */ view_size, /* item_size */ array->item_size, /* data */ view_data };
	return view;
}

CArrayView carray_view_create(u32 size, u32 item_size, void *data) {
	CArrayView view = { size, item_size, ( u8 * )data };
	return view;
}

u32 carray_resize(CArray *array, u32 new_capacity) {
	if (new_capacity <= array->size) {
		// Shrink down the size, but keep capacity the same.
		array->size = new_capacity;
		return array->capacity;
	}

	if (new_capacity <= array->capacity) {
		// Shrink down the size, but keep capacity the same.
		array->size = new_capacity;
		return array->capacity;
	}

	// Allocate at least N items.
	// NOTE(nilsoncore): Check if this optimisation is worth it.
	new_capacity = (new_capacity >= CARRAY_RESIZE_MIN_CAPACITY) ? new_capacity : CARRAY_RESIZE_MIN_CAPACITY;
	array->data = Reallocate(array->allocator, array->data, array->capacity * array->item_size, new_capacity * array->item_size, u8);
	array->capacity = new_capacity;
	return array->capacity;
}

// Returns newly added item's index.
u32 carray_add(CArray *array, void *item) {
	if (array->size + 1 > array->capacity) {
		carray_resize(array, array->capacity * 2);
	}

	const u32 current_index = array->size * array->item_size;
	memcpy( &array->data[ current_index ], item, array->item_size );
	array->size++;

	return current_index;
}

// Returns number of added items.
u32 carray_add_many(CArray *array, CArrayView source) {
	// 1 enlargement might be not enough.
	// TODO(nilsonragee): Sufficient size might be calculated once, but right now I'm lazy.
	AssertMessage(array->item_size == source.item_size, "Item sizes must match");
	while (array->size + source.size > array->capacity) {
		carray_resize(array, array->capacity * 2);
	}

	const u32 current_index = array->size * array->item_size;
	const u32 copy_size = source.size * source.item_size;
	memcpy( &array->data[ current_index ], source.data, copy_size );
	array->size += source.size;

	// Assume we always add all items since this is a dynamic array case.
	return source.size;
}

u32 carray_add_repeat(CArray *array, void *item, u32 count) {
	const u32 space_left = array->capacity - array->size;
	// const u32 items_to_add = (space_left <= count) ? space_left : count - space_left;
	const u32 items_to_add = (space_left <= count) ? space_left : count;
	if (items_to_add < 1)
		return items_to_add;

	u32 current_index = array->size * array->item_size;
	const u32 last_index = current_index + items_to_add;
	while ( current_index < last_index ) {
		memcpy( &array->data[ current_index ], item, array->item_size );
		current_index += 1;
	}

	array->size += items_to_add;
	return items_to_add;
}

u32 carray_add_from_array(CArray *destination, CArray *source, u32 source_offset, u32 count) {
	AssertMessage( false, "Not implemented" );
/*
	const bool within_source_array = (source->size >= source_offset + count);
	const u32 source_items_to_add = (within_source_array) ? count : source->size - source_offset - count;
	if (source_items_to_add < 1)
		return source_items_to_add;

	const u32 space_left = destination->capacity - destination->size;
	// const u32 items_to_add = (space_left >= source_items_to_add) ? source_items_to_add : source_items_to_add - space_left;
	const u32 items_to_add = (space_left <= source_items_to_add) ? space_left : source_items_to_add;
	if (items_to_add < 1)
		return items_to_add;

	u32 destination_item_idx = destination->size;
	u32 source_item_idx = source_offset;
	for (u32 item_idx = 0; item_idx < items_to_add; item_idx += 1) {
		destination->data[destination_item_idx] = source->data[source_item_idx];
		destination_item_idx += 1;
		source_item_idx += 1;
	}

	destination->size += items_to_add;
	return items_to_add;
*/
	return 0;
}

bool carray_pop(CArray *array, void *out_item) {
    if (array->size < 1)
    	return false;

    if (out_item) {
    	const u32 current_index = array->size * array->item_size;
    	memcpy( out_item, &array->data[ current_index - 1], array->item_size );
    }

    array->size -= 1;
    return true;
}

void carray_clear(CArray *array, bool zero_memory) {
    array->size = 0;
    if (zero_memory)
    	memset(array->data, 0, (u64)array->capacity * array->item_size);
}

bool carray_free(CArray *array) {
    if (!array->data)         return false;
    if (array->capacity < 1)  return false;

    Deallocate(array->allocator, array->data);
    array->size = 0;
    array->capacity = 0;
    return true;
}

bool carray_contains(CArray *array, void *item) {
    for (u32 index = 0; index < array->size; index += 1) {
    	const void *array_item = &array->data[ index * array->item_size ];
        const int memory_diff = memcmp( array_item, item, array->item_size );
        if ( memory_diff == 0 )  return true;
    }
    return false;
}

void * carray_find(CArray *array, void *item) {
    for (u32 index = 0; index < array->size; index++) {
        void *array_item = &array->data[ index * array->item_size ];
        const int memory_diff = memcmp( array_item, item, array->item_size );
        if ( memory_diff == 0 )  return array_item;
    }
    return nullptr;
}

bool carray_remove(CArray *array, void *item) {
	void *array_item = carray_find( array, item );
	if ( !array_item )
		return false;

	memset( array_item, 0, array->item_size );
	return true;
}

void carray_remove_at_pointer(CArray *array, void *array_item) {
	bool in_min_bounds = ( array_item >= array->data );
	bool in_max_bounds = ( array_item < ( array->data + array->size * array->item_size ) );
	Assert( in_min_bounds && in_max_bounds );
	if ( !in_min_bounds || !in_max_bounds )
		return;

	memset( array_item, 0, array->item_size );
}

void carray_remove_at_index(CArray *array, u32 index) {
	bool in_min_bounds = ( index >= 0 );
	bool in_max_bounds = ( index < array->size );
	Assert( in_min_bounds && in_max_bounds );
	if ( !in_min_bounds || !in_max_bounds )
		return;

	void *array_item = carray_at( array, index );
	memset( array_item, 0, array->item_size );
}
