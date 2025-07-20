#ifndef QLIGHT_COMMON_H
#define QLIGHT_COMMON_H

#include "types.h"
#include "allocator.h"

// #define For( count )  for ( uint32_t it = 0; it < count; count += 1 )

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