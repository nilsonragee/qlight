#include "common.h"

// Sections:
// [SECTION] Function: clamp
// [SECTION] Function: min2
// [SECTION] Function: max2
// [SECTION] Function: abs
// [SECTION] Function: bool_to_string

//-----------------------------------------------------------------------------
// [SECTION] Function: clamp
//-----------------------------------------------------------------------------

s8 QL_clamp(s8 x, s8 min, s8 max) {
	if (x < min)  return min;
	if (x > max)  return max;
	return x;
}

u8 QL_clamp(u8 x, u8 min, u8 max) {
	if (x < min)  return min;
	if (x > max)  return max;
	return x;
}

s16 QL_clamp(s16 x, s16 min, s16 max) {
	if (x < min)  return min;
	if (x > max)  return max;
	return x;
}

u16 QL_clamp(u16 x, u16 min, u16 max) {
	if (x < min)  return min;
	if (x > max)  return max;
	return x;
}

s32 QL_clamp(s32 x, s32 min, s32 max) {
	if (x < min)  return min;
	if (x > max)  return max;
	return x;
}

u32 QL_clamp(u32 x, u32 min, u32 max) {
	if (x < min)  return min;
	if (x > max)  return max;
	return x;
}

s64 QL_clamp(s64 x, s64 min, s64 max) {
	if (x < min)  return min;
	if (x > max)  return max;
	return x;
}

u64 QL_clamp(u64 x, u64 min, u64 max) {
	if (x < min)  return min;
	if (x > max)  return max;
	return x;
}

f32 QL_clamp(f32 x, f32 min, f32 max) {
	if (x < min)  return min;
	if (x > max)  return max;
	return x;
}

f64 QL_clamp(f64 x, f64 min, f64 max) {
	if (x < min)  return min;
	if (x > max)  return max;
	return x;
}

//-----------------------------------------------------------------------------
// [SECTION] Function: min2
//-----------------------------------------------------------------------------

s8 QL_min2(s8 a, s8 b) { return (a < b) ? a : b; }
u8 QL_min2(u8 a, u8 b) { return (a < b) ? a : b; }
s16 QL_min2(s16 a, s16 b) { return (a < b) ? a : b; }
u16 QL_min2(u16 a, u16 b) { return (a < b) ? a : b; }
s32 QL_min2(s32 a, s32 b) { return (a < b) ? a : b; }
u32 QL_min2(u32 a, u32 b) { return (a < b) ? a : b; }
s64 QL_min2(s64 a, s64 b) { return (a < b) ? a : b; }
u64 QL_min2(u64 a, u64 b) { return (a < b) ? a : b; }
f32 QL_min2(f32 a, f32 b) { return (a < b) ? a : b; }
f64 QL_min2(f64 a, f64 b) { return (a < b) ? a : b; }

//-----------------------------------------------------------------------------
// [SECTION] Function: max2
//-----------------------------------------------------------------------------

s8 QL_max2(s8 a, s8 b) { return (a > b) ? a : b; }
u8 QL_max2(u8 a, u8 b) { return (a > b) ? a : b; }
s16 QL_max2(s16 a, s16 b) { return (a > b) ? a : b; }
u16 QL_max2(u16 a, u16 b) { return (a > b) ? a : b; }
s32 QL_max2(s32 a, s32 b) { return (a > b) ? a : b; }
u32 QL_max2(u32 a, u32 b) { return (a > b) ? a : b; }
s64 QL_max2(s64 a, s64 b) { return (a > b) ? a : b; }
u64 QL_max2(u64 a, u64 b) { return (a > b) ? a : b; }
f32 QL_max2(f32 a, f32 b) { return (a > b) ? a : b; }
f64 QL_max2(f64 a, f64 b) { return (a > b) ? a : b; }

//-----------------------------------------------------------------------------
// [SECTION] Function: abs
//-----------------------------------------------------------------------------

s8 QL_abs(s8 x) { return (x > 0) ? x : -x; }
s16 QL_abs(s16 x) { return (x > 0) ? x : -x; }
s32 QL_abs(s32 x) { return (x > 0) ? x : -x; }
s64 QL_abs(s64 x) { return (x > 0) ? x : -x; }
f32 QL_abs(f32 x) { return (x > 0.0f) ? x : -x; }
f64 QL_abs(f64 x) { return (x > 0.0) ? x : -x; }

//-----------------------------------------------------------------------------
// [SECTION] Function: bool_to_string
//-----------------------------------------------------------------------------

const char *QL_bool_to_string(bool value) {
	if (value)  return "true";
	return "false";
}
