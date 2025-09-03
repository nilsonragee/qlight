#ifndef QLIGHT_ARRAY_H
#define QLIGHT_ARRAY_H

#include "common.h"

// #define ARRAY_RESIZE_MIN_CAPACITY 64u
constexpr const u32 ARRAY_RESIZE_MIN_CAPACITY = 64;

#define StringViewFormat "%.*s"
#define StringViewArgument(array_view) array_view.size, array_view.data

template <typename T>
struct Array {
	Allocator *allocator;
	u32 size;
	u32 capacity;
	T *data;
};

template <typename T>
struct ArrayView {
	u32 size;
	T *data;
};

template <typename T>
Array<T> array_new(Allocator *allocator, u32 initial_capacity) {
	Array<T> array;
	array.allocator = allocator;
	array.size = 0;
	array.capacity = initial_capacity;
	array.data = (initial_capacity > 0) ? TemplateAllocate(allocator, array.capacity, T) : NULL;
	return array;
}

template <typename T>
Array<T> array_new(Allocator *allocator, ArrayView<T> source) {
	Array<T> array;
	array.allocator = allocator;
	array.size = source.size;
	array.capacity = source.size;
	if ( source.size > 0 ) {
		array.data = TemplateAllocate(allocator, array.capacity, T);
		array_add_many(&array, source);
	} else {
		array.data = NULL;
	}
	return array;
}

template < typename T >
static ArrayView< T > array_view_impl( T* data, u32 data_size, u32 size = 0, u32 offset = 0 ) {
	ArrayView< T > view;
	if ( offset > size ) {
		view.data = NULL;
		view.size = 0;
		return view;
	}

	const u32 size_available = data_size - offset;
	const bool count_to_end = (size == 0);
	if (count_to_end) {
		view.size = size_available;
	} else {
		AssertMessage(size <= size_available, "ArrayView size is out of bounds");
		// If view size is overflowing, clamp it to the end of array.
		view.size = (size <= size_available) ? size : size_available;
	}

	view.data = data + offset;
	return view;
}

// count = 0 -- means count to the end of the array's size.
template <typename T>
ArrayView<T> array_view(Array<T> *array, u32 offset = 0, u32 count = 0) {
	AssertMessage(offset <= array->size, "ArrayView array offset is out of bounds");
	return array_view_impl( array->data, array->size, count, offset );
}

template <typename T>
ArrayView<T> array_view(T *data, u32 data_size, u32 offset = 0, u32 count = 0) {
	AssertMessage(offset <= data_size, "ArrayView data offset is out of bounds");
	return array_view_impl( data, data_size, count, offset );
}

template <typename T>
ArrayView<T> array_view_slice(ArrayView<T> view, u32 offset, u32 count = 0) {
	if (offset > view->size) {
		// If array offset is out of bounds, return everything zeroed.
		ArrayView<T> slice = { /* size */ 0, /* data */ NULL };
		return slice;
	}

	T *slice_data = view->data + offset;

	const u32 slice_size_available = view->size - offset;
	const bool count_to_end = (count == 0);
	u32 slice_size;
	if (count_to_end) {
		slice_size = slice_size_available;
	} else {
		AssertMessage(count <= slice_size_available, "ArrayView size is out of bounds");
		// If view size is overflowing, clamp it to the end of array.
		slice_size = (count <= slice_size_available) ? count : slice_size_available;
	}

	ArrayView<T> slice = { /* size */ slice_size, /* data */ slice_data };
	return slice;
}

template <typename T>
u32 array_resize(Array<T> *array, u32 new_capacity) {
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
	new_capacity = (new_capacity >= ARRAY_RESIZE_MIN_CAPACITY) ? new_capacity : ARRAY_RESIZE_MIN_CAPACITY;
	array->data = TemplateReallocate(array->allocator, array->data, array->capacity, new_capacity, T);
	array->capacity = new_capacity;
	return array->capacity;
}

// Returns newly added item's index.
template <typename T>
u32 array_add(Array<T> *array, T item) {
    if (array->size + 1 > array->capacity) {
    	array_resize(array, array->capacity * 2);
    }

    array->data[array->size] = item;
    const u32 current_index = array->size;
    array->size++;

    return current_index;
}

// Returns number of added items.
template <typename T>
u32 array_add_many(Array<T> *array, ArrayView<T> source) {
	// 1 enlargement might be not enough.
	// TODO(nilsonragee): Sufficient size might be calculated once, but right now I'm lazy.
    while (array->size + source.size > array->capacity) {
    	array_resize(array, array->capacity * 2);
    }

	u32 source_idx = 0;
	while (source_idx < source.size) {
		array->data[array->size] = source.data[source_idx];
		array->size += 1;
		source_idx += 1;
	}

	// Assume we always add all items since this is a dynamic array case.
    return source.size;
}

template <typename T>
u32 array_add_repeat(Array<T> *array, T item, u32 count) {
	const u32 space_left = array->capacity - array->size;
	// const u32 items_to_add = (space_left <= count) ? space_left : count - space_left;
	const u32 items_to_add = (space_left <= count) ? space_left : count;
	if (items_to_add < 1)
		return items_to_add;

	for (u32 item_idx = 0; item_idx < items_to_add; item_idx += 1) {
		array->data[item_idx] = item;
	}

	array->size += items_to_add;
	return items_to_add;
}

template <typename T>
u32 array_add_from_array(Array<T> *destination, Array<T> *source, u32 source_offset, u32 count) {
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
}

template <typename T>
bool array_pop(Array<T> *array, T *out_item) {
    if (array->size < 1)
    	return false;

    if (out_item)
    	*out_item = array->data[array->size - 1];

    array->size--;
    return true;
}

#include <string.h> // memset() -- @TODO: Remove

template <typename T>
void array_clear(Array<T> *array, bool zero_memory = false) {
    if (zero_memory)
    	memset(array->data, 0, array->size * sizeof(T));

	array->size = 0;
}

template <typename T>
bool array_free(Array<T> *array, bool zero_memory = false) {
    if (!array->data)         return false;
    if (array->capacity < 1)  return false;

	array_clear(array, zero_memory);
    Deallocate(array->allocator, array->data);
    array->capacity = 0;
    return true;
}

template <typename T>
bool array_contains(Array<T> *array, T item) {
    for (u32 index = 0; index < array->size; index++) {
        if (array->data[index] == item)  return true;
    }
    return false;
}

template <typename T>
T* array_find(Array<T> *array, T item) {
    for (u32 index = 0; index < array->size; index++) {
        if (array->data[index] == item)  return &array->data[index];
    }
    return NULL;
}

#endif /* QLIGHT_ARRAY_H */
