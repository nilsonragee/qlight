#include <stdlib.h> // malloc(), realloc(), free()
#include <string.h>

#include "allocator.h"

#undef max
#define max(a, b) (a > b) ? a : b

static System_Allocator g_qlight_sys_allocator;
Allocator *sys_allocator = &g_qlight_sys_allocator;

u8 *Allocator::request_allocate(u64 count, u64 size, CallerInfo caller) {
	u8 *allocated = do_allocate(count, size, caller);
	AssertMessage(allocated, "Failed to allocate");
	return allocated;
}

u8 *Allocator::request_reallocate(void *memory_pointer, u64 old_count, u64 new_count, u64 size, CallerInfo caller) {
	AssertMessage(memory_pointer, "Memory pointer is NULL");
	u8 *reallocated = do_reallocate(memory_pointer, old_count, new_count, size, caller);
	AssertMessage(reallocated, "Failed to reallocate");
	return reallocated;
}

void Allocator::request_deallocate(void *memory_pointer, CallerInfo caller) {
	AssertMessage(memory_pointer, "Memory pointer is NULL");
	do_deallocate(memory_pointer, caller);
}

u8 *System_Allocator::do_allocate(u64 count, u64 size, CallerInfo caller) {
	return (u8 *) malloc(count * size);
}

u8 *System_Allocator::do_reallocate(void *memory_pointer, u64 old_count, u64 new_count, u64 size, CallerInfo caller) {
	return (u8 *) realloc(memory_pointer, new_count * size);
	// u8 *allocated = do_allocate(new_count, size, caller);
	// memcpy(allocated, memory_pointer, old_count * size);
	// do_free(memory_pointer, caller);
	// return allocated;
}

void System_Allocator::do_deallocate(void *memory_pointer, CallerInfo caller) {
	free(memory_pointer);
}

static bool power_of_2(u64 x) {
    // u64 x   = 4
    //     x   = 0x100
    //     x-1 = 0x011
    // x & x-1 = 0x000
    //         => x (4) is power of 2.
    return (x & (x-1)) == 0;
}

void Linear_Allocator::init(void *memory_pointer, u64 size) {
    AssertMessage(memory_pointer, "Trying to initialize 'Linear_Allocator' with NULL pointer");

    memory_start = (u8 *)memory_pointer;
    memory_end = memory_start + size;
    cursor = memory_start;
    cursor_max = cursor;

    allocated = size;
}

void Linear_Allocator::deinit() {
    AssertMessage(memory_start, "Allocator is not initialized or already deinitialized");
    free(memory_start);

    memory_start = NULL;
    memory_end = NULL;
}

void Linear_Allocator::reset(bool zero_memory) {
    AssertMessage(memory_start, "Allocator is not initialized or already deinitialized");
    cursor = memory_start;
    // maybe keep max usage info on clear?
    // cursor_max = cursor;

    if (zero_memory)  memset(memory_start, 0, allocated * sizeof(u8));
}

u64 Linear_Allocator::occupied() {
	return cursor - memory_start;
}

u8 *Linear_Allocator::do_allocate(u64 count, u64 size, CallerInfo caller) {
    AssertMessage(power_of_2(size), "Allocation size (alignment) must be power of 2");
    u8 *aligned = (u8 *)(((u64)(cursor + size - 1)) & ~(size - 1));
    if (aligned + count > memory_end)  return NULL;

    cursor = aligned + count;
    cursor_max = max(cursor, cursor_max);
    return aligned;
}

u8 *Linear_Allocator::do_reallocate(void *memory_pointer, u64 old_count, u64 new_count, u64 size, CallerInfo caller) {
    if (!memory_pointer)  return do_allocate(new_count, size, caller);

    if (memory_pointer == cursor - old_count && (u64)memory_pointer % size == 0) {
        u8 *new_cursor = cursor - old_count + new_count;

        if (new_cursor > memory_end)  return NULL;
        AssertMessage(new_cursor < cursor, "New cursor is below the current cursor for some reason");

        cursor = new_cursor;
        cursor_max = max(cursor, cursor_max);
        return (u8 *)memory_pointer;
    }

    u8 *new_memory_pointer = do_allocate(new_count, size, caller);
    if (!new_memory_pointer)  return NULL;

    memcpy(new_memory_pointer, memory_pointer, old_count);
    return new_memory_pointer;
}

void Linear_Allocator::do_deallocate(void *memory_pointer, CallerInfo caller) {
    if (!memory_pointer) {
        reset(false);
        return;
    }

    AssertMessage(false, "Trying to deallocate with 'Linear_Allocator'");
}
