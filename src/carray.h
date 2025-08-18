#ifndef QLIGHT_CARRAY_H
#define QLIGHT_CARRAY_H

#include "common.h"

// #define ARRAY_RESIZE_MIN_CAPACITY 64u
constexpr const u32 CARRAY_RESIZE_MIN_CAPACITY = 64;

#define CStringViewFormat "%.*s"
#define CStringViewArgument(array_view) array_view.size, array_view.data

struct CArray {
	Allocator *allocator;
	u32 size;
	u32 capacity;
	u8 *data;
	u32 item_size;
};

struct CArrayView {
	u32 size;
	u32 item_size;
	u8 *data;
};

extern "C" {

inline u8 * carray_at(CArray *array, u32 index) {
	return &array->data[ index * array->item_size ];
};

inline u32 carray_index_of(CArray *array, void *item) {
	u32 memory_offset = ( u32 )( ( u8 * )item - ( u8 * )array->data );
	u32 index = memory_offset / array->item_size;
	return index;
};

// count = 0 -- means count to the end of the array's size.
CArrayView get_carray_view(CArray *array, u32 offset = 0, u32 count = 0);
CArrayView carray_view(CArray *array, u32 offset = 0, u32 count = 0);
CArrayView carray_view_create(u32 size, u32 item_size, void *data);

CArray carray_new(Allocator *allocator, u32 item_size, u32 initial_capacity);
 u32   carray_resize(CArray *array, u32 new_capacity);

// Returns newly added item's index.
 u32   carray_add(CArray *array, void *item);
// Returns number of added items.
 u32   carray_add_many(CArray *array, CArrayView source);

 u32   carray_add_repeat(CArray *array, void *item, u32 count);
 u32   carray_add_from_array(CArray *destination, CArray *source, u32 source_offset, u32 count);
bool   carray_pop(CArray *array, void *out_item);
void   carray_clear(CArray *array, bool zero_memory = false);
bool   carray_free(CArray *array);
bool   carray_contains(CArray *array, void *item);
void * carray_find(CArray *array, void *item);
void   carray_remove_at_pointer(CArray *array, void *item);
void   carray_remove_at_index(CArray *array, u32 index);

} /* extern "C" */

#endif /* QLIGHT_CARRAY_H */