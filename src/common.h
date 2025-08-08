#ifndef QLIGHT_COMMON_H
#define QLIGHT_COMMON_H

#include "types.h"
#include "allocator.h"

// `( count & 0 )` is a trick to make compiler do type deduction from `count` variable.
// If you would assign to 0, the compiler would infer from the literal - 0 is `int`.
/*
	ForNamed( i, 3 ) {
		log_info( "i=%u", i );
	}

	is equivalent to:

	for ( auto i = ( 3 & 0 ); i < 3; i += 1 ) {
		log_info( "i=%u", i );
	}
*/
#define ForNamed( variable, count )  for ( auto variable = ( count & 0 ); variable < count; variable += 1 )

// Use `s64` so the index can not underflow if `count` is unsigned.
// It could be countered by checking for `count`'s type max value, but how do you get the type?
// Too much hustle, keep it simple.
/*
	ForNamedBackwards( i, 3 ) {
		log_info( "i=%u", i );
	}

	is equivalent to:

	for ( s64 i = 3 - 1; i >= 0; i -= 1 ) {
		log_info( "i=%u", i );
	}
*/
#define ForNamedBackwards( variable, count )  for ( s64 variable = count - 1; variable >= 0; variable -= 1 )

/*
	For( 3 ) {
		log_info( "it_index=%u", it_index );
	}

	is equivalent to:

	for ( auto it_index = ( 3 & 0 ); it_index < 3; it_index += 1 ) {
		log_info( "it_index=%u", it_index );
	}
*/
#define For( count )  ForNamed( it_index, count )

/*
	ForBackwards( 3 ) {
		log_info( "it_index=%u", it_index );
	}

	is equivalent to:

	for ( auto it_index = 3 - 1; it_index >= 0; it_index -= 1 ) {
		log_info( "it_index=%u", it_index );
	}
*/
#define ForBackwards( count )  ForNamedBackwards( it_index, count )

#define For2( count )  ForNamed( it2_index, count )
#define For2Backwards( count )  ForNamedBackwards( it2_index, count )

#define For3( count )  ForNamed( it3_index, count )
#define For3Backwards( count )  ForNamedBackwards( it3_index, count )

/*
	StringView_ASCII str = "Hello World";
	ForIt( str.data, str.size ) {
		log_info( "str[%2u]: '%c'", it_index, *it );
	}}

	is equivalent to:

	StringView_ASCII str = "Hello World";
	for ( auto it_index = str.size; it_index < str.size; it_index += 1 ) {
		auto *it = &pointer[ it_index ];
		{
			log_info( "str[%2u]: '%c'", it_index, *it );
		}
	}
*/
#define ForIt( pointer, count )  \
		For( count ) {  \
			auto *it = &pointer[ it_index ];

/*
	StringView_ASCII str = "Hello World";
	ForItBackwards( str.data, str.size ) {
		log_info( "str[%2u]: '%c'", it_index, *it );
	}}

	is equivalent to:

	StringView_ASCII str = "Hello World";
	for ( s64 it_index = str.size - 1; it_index >= 0; it_index -= 1 ) {
		auto *it = &pointer[ it_index ];
		{
			log_info( "str[%2u]: '%c'", it_index, *it );
		}
	}
*/
#define ForItBackwards( pointer, count )  \
		ForBackwards( count ) {  \
			auto *it = &pointer[ it_index ];


#define ForIt2( pointer, count )  \
		For2( count ) {  \
			auto *it2 = &pointer[ it2_index ];

#define ForIt2Backwards( pointer, count )  \
		For2Backwards( count ) {  \
			auto *it2 = &pointer[ it2_index ];


#define ForIt3( pointer, count )  \
		For3( count ) {  \
			auto *it3 = &pointer[ it3_index ];

#define ForIt3Backwards( pointer, count )  \
		For3Backwards( count ) {  \
			auto *it3 = &pointer[ it3_index ];


#define ARRAY_SIZE( array )  sizeof( array ) / sizeof( array[ 0 ] )

s8 QL_clamp(s8 x, s8 min, s8 max);
u8 QL_clamp(u8 x, u8 min, u8 max);
s16 QL_clamp(s16 x, s16 min, s16 max);
u16 QL_clamp(u16 x, u16 min, u16 max);
s32 QL_clamp(s32 x, s32 min, s32 max);
u32 QL_clamp(u32 x, u32 min, u32 max);
s64 QL_clamp(s64 x, s64 min, s64 max);
u64 QL_clamp(u64 x, u64 min, u64 max);
f32 QL_clamp(f32 x, f32 min, f32 max);
f64 QL_clamp(f64 x, f64 min, f64 max);

s8 QL_min2(s8 a, s8 b);
u8 QL_min2(u8 a, u8 b);
s16 QL_min2(s16 a, s16 b);
u16 QL_min2(u16 a, u16 b);
s32 QL_min2(s32 a, s32 b);
u32 QL_min2(u32 a, u32 b);
s64 QL_min2(s64 a, s64 b);
u64 QL_min2(u64 a, u64 b);
f32 QL_min2(f32 a, f32 b);
f64 QL_min2(f64 a, f64 b);

s8 QL_max2(s8 a, s8 b);
u8 QL_max2(u8 a, u8 b);
s16 QL_max2(s16 a, s16 b);
u16 QL_max2(u16 a, u16 b);
s32 QL_max2(s32 a, s32 b);
u32 QL_max2(u32 a, u32 b);
s64 QL_max2(s64 a, s64 b);
u64 QL_max2(u64 a, u64 b);
f32 QL_max2(f32 a, f32 b);
f64 QL_max2(f64 a, f64 b);

s8 QL_abs(s8 x);
s16 QL_abs(s16 x);
s32 QL_abs(s32 x);
s64 QL_abs(s64 x);
f32 QL_abs(f32 x);
f64 QL_abs(f64 x);

const char *QL_bool_to_string(bool value);

#endif /* QLIGHT_COMMON_H */