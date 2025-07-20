#ifndef QLIGHT_ALLOCATOR_H
#define QLIGHT_ALLOCATOR_H

#include "platform.h"

#if defined(QLIGHT_PLATFORM_WINDOWS)
	#define QL_AllocatorEmptyCaller()      CallerInfo { "(none)", __FUNCSIG__, __FILE__, __LINE__ }
	#define QL_AllocatorTypeCaller(T)      CallerInfo { "'" #T "'", __FUNCSIG__, __FILE__, __LINE__ }
	#define QL_AllocatorTemplateCaller(T)  CallerInfo { get_type_name<T>(), __FUNCSIG__, __FILE__, __LINE__ }
#elif defined(QLIGHT_PLATFORM_LINUX)
	#define QL_AllocatorEmptyCaller()      CallerInfo { "(none)", __PRETTY_FUNCTION__, __FILE__, __LINE__ }
	#define QL_AllocatorTypeCaller(T)      CallerInfo { "'" #T "'", __PRETTY_FUNCTION__, __FILE__, __LINE__ }
	#define QL_AllocatorTemplateCaller(T)  CallerInfo { get_type_name<T>(), __PRETTY_FUNCTION__, __FILE__, __LINE__ }
#endif

/*
	Base allocator functions macros.
	These are designed to be wrapped around with another macro.
	Avoid using these macros directly.
*/

#define QL_Allocate(allocator, count, size, caller) \
	allocator->request_allocate( \
		count, \
		size, \
		caller \
	)

#define QL_Reallocate(allocator, memory_pointer, old_count, new_count, size, caller) \
	allocator->request_reallocate( \
		memory_pointer, \
		old_count, \
		new_count, \
		size, \
		caller \
	)

#define QL_Deallocate(allocator, memory_pointer, caller) \
	allocator->request_deallocate( \
		memory_pointer, \
		caller \
	)

/*
	Macro wrappings around base allocator functions macros.
	These are designed to be used in source code.
	Use these macros to work with allocators.
*/

#define Allocate(allocator, count, T) \
	(T *) QL_Allocate(allocator, count, sizeof(T), QL_AllocatorTypeCaller(#T))

#define Reallocate(allocator, memory_pointer, old_count, new_count, T) \
	(T *) QL_Reallocate(allocator, memory_pointer, old_count, new_count, sizeof(T), QL_AllocatorTypeCaller(#T))

#define Deallocate(allocator, memory_pointer) \
	QL_Deallocate(allocator, memory_pointer, QL_AllocatorEmptyCaller())

/*
	Macro wrappings for use inside C++ templates.
	These macros use helper template functions instead of macro trickery
	to get the type name string representation,
	so it's required that these are used inside templates.
*/

#define TemplateAllocate(allocator, count, T) \
	(T *) QL_Allocate(allocator, count, sizeof(T), QL_AllocatorTemplateCaller(T))

#define TemplateReallocate(allocator, memory_pointer, old_count, new_count, T) \
	(T *) QL_Reallocate(allocator, memory_pointer, old_count, new_count, sizeof(T), QL_AllocatorTemplateCaller(T))

struct CallerInfo {
	const char *type;
	const char *function;
	const char *file;
	long line;
};

struct Allocator {
	u8 *request_allocate(u64 count, u64 size, CallerInfo caller);
	u8 *request_reallocate(void *memory_pointer, u64 old_count, u64 new_count, u64 size, CallerInfo caller);
	void request_deallocate(void *memory_pointer, CallerInfo caller);

	virtual u8 *do_allocate(u64 count, u64 size, CallerInfo caller) = 0;
	virtual u8 *do_reallocate(void *memory_pointer, u64 old_count, u64 new_count, u64 size, CallerInfo caller) = 0;
	virtual void do_deallocate(void *memory_pointer, CallerInfo caller) = 0;
};

extern Allocator *sys_allocator;

struct System_Allocator : Allocator {
	u8 *do_allocate(u64 count, u64 size, CallerInfo caller);
	u8 *do_reallocate(void *memory_pointer, u64 old_count, u64 new_count, u64 size, CallerInfo caller);
	void do_deallocate(void *memory_pointer, CallerInfo caller);
};

struct Linear_Allocator : Allocator {
	u8 *memory_start = NULL;
	u8 *memory_end = NULL;
	u8 *cursor = NULL;
	u8 *cursor_max = NULL;

	u64 allocated = 0;

	u8 *do_allocate(u64 count, u64 size, CallerInfo caller);
	u8 *do_reallocate(void *memory_pointer, u64 old_count, u64 new_count, u64 size, CallerInfo caller);
	void do_deallocate(void *memory_pointer, CallerInfo caller);

	void init(void *memory_pointer, u64 size);
	void deinit();
	void reset(bool zero_memory);

	u64 occupied();
};

#endif /* QLIGHT_ALLOCATOR_H */