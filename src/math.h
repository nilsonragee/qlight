#ifndef QLIGHT_MATH_H
#define QLIGHT_MATH_H

#include "types.h"

#undef min
#undef max

// Sections:
// [SECTION] Vectors / Matrices forward declarations
// [SECTION] Vector2: s8 -> u8 -> s16 -> u16 -> s32 -> u32 -> s64 -> u64 -> f32 -> f64
// [SECTION] Vector3: s8 -> u8 -> s16 -> u16 -> s32 -> u32 -> s64 -> u64 -> f32 -> f64
// [SECTION] Vector4: s8 -> u8 -> s16 -> u16 -> s32 -> u32 -> s64 -> u64 -> f32 -> f64

//-----------------------------------------------------------------------------
// [SECTION] Vectors/ Matrices forward declarations
//-----------------------------------------------------------------------------

struct Vector2_s8;
struct Vector2_u8;
struct Vector2_s16;
struct Vector2_u16;
struct Vector2_s32;
struct Vector2_u32;
struct Vector2_s64;
struct Vector2_u64;
struct Vector2_f32;
struct Vector2_f64;

struct Vector3_s8;
struct Vector3_u8;
struct Vector3_s16;
struct Vector3_u16;
struct Vector3_s32;
struct Vector3_u32;
struct Vector3_s64;
struct Vector3_u64;
struct Vector3_f32;
struct Vector3_f64;

struct Vector4_s8;
struct Vector4_u8;
struct Vector4_s16;
struct Vector4_u16;
struct Vector4_s32;
struct Vector4_u32;
struct Vector4_s64;
struct Vector4_u64;
struct Vector4_f32;
struct Vector4_f64;

// Default vector mappings:
typedef  Vector2_f32  Vector2; // `Vector2` = `Vector2_f32` by default.
typedef  Vector3_f32  Vector3; // `Vector3` = `Vector3_f32` by default.
typedef  Vector4_f32  Vector4; // `Vector4` = `Vector4_f32` by default.

struct Vector3_f32_XYZ {
	float x;
	float y;
	float z;
};

struct Vector4_f32_XYZW {
	float x;
	float y;
	float z;
	float w;
};

struct Matrix3x3_f32_XYZ {
	Vector3_f32_XYZ columns[3];
};

struct Matrix4x4_f32_XYZW {
	Vector4_f32_XYZW columns[4];
};

struct Matrix3x3_f32;
struct Matrix3x3_f64;

struct Matrix4x4_f32;
struct Matrix4x4_f64;

// Default matrix mappings:
typedef  Matrix3x3_f32  Matrix3x3;
typedef  Matrix4x4_f32  Matrix4x4;

constexpr f32 PI = 3.141592653589793f;
constexpr f32 PI2 = 6.283185307179586f;

f32 radians( f32 degrees );
f32 degrees( f32 radians );

f32 inverse_sqrt_fast( f32 x );
f32 inverse_sqrt_refined( f32 x );
f32 inverse_sqrt_general( f32 x );
inline f32 inverse_sqrt( f32 x )  { return inverse_sqrt_fast( x ); }

Vector3_f32 normalize( Vector3_f32 v );
Vector2_f32 cross( Vector2_f32 lhs, Vector2_f32 rhs );
Vector3_f32 cross( Vector3_f32 lhs, Vector3_f32 rhs );
f32 dot( Vector2_f32 lhs, Vector2_f32 rhs );
f32 dot( Vector3_f32 lhs, Vector3_f32 rhs );
f32 dot( Vector4_f32 lhs, Vector4_f32 rhs );

//-----------------------------------------------------------------------------
// [SECTION] Vector2: s8 -> u8 -> s16 -> u16 -> s32 -> u32 -> s64 -> u64 -> f32 -> f64
//-----------------------------------------------------------------------------

struct Vector2_s8 {
	union { s8 x; s8 r; s8 width;  };
	union { s8 y; s8 g; s8 height; };

	bool operator == (Vector2_s8 rhs);
	bool operator == (s8 rhs);

	s8 & operator [] (int index);

	Vector2_s8 & operator += (Vector2_s8 rhs);
	Vector2_s8 & operator -= (Vector2_s8 rhs);
	Vector2_s8 & operator *= (Vector2_s8 rhs);
	Vector2_s8 & operator /= (Vector2_s8 rhs);
	Vector2_s8 & operator %= (Vector2_s8 rhs);

	Vector2_s8 & operator += (s8 rhs);
	Vector2_s8 & operator -= (s8 rhs);
	Vector2_s8 & operator *= (s8 rhs);
	Vector2_s8 & operator /= (s8 rhs);
	Vector2_s8 & operator %= (s8 rhs);

	Vector2_s8 operator + (Vector2_s8 rhs);
	Vector2_s8 operator - (Vector2_s8 rhs);
	Vector2_s8 operator * (Vector2_s8 rhs);
	Vector2_s8 operator / (Vector2_s8 rhs);
	Vector2_s8 operator % (Vector2_s8 rhs);

	Vector2_s8 operator + (s8 rhs);
	Vector2_s8 operator - (s8 rhs);
	Vector2_s8 operator * (s8 rhs);
	Vector2_s8 operator / (s8 rhs);
	Vector2_s8 operator % (s8 rhs);
};

struct Vector2_u8 {
	union { u8 x; u8 r; u8 width;  };
	union { u8 y; u8 g; u8 height; };

	bool operator == (Vector2_u8 rhs);
	bool operator == (u8 rhs);

	u8 & operator [] (int index);

	Vector2_u8 & operator += (Vector2_u8 rhs);
	Vector2_u8 & operator -= (Vector2_u8 rhs);
	Vector2_u8 & operator *= (Vector2_u8 rhs);
	Vector2_u8 & operator /= (Vector2_u8 rhs);
	Vector2_u8 & operator %= (Vector2_u8 rhs);

	Vector2_u8 & operator += (u8 rhs);
	Vector2_u8 & operator -= (u8 rhs);
	Vector2_u8 & operator *= (u8 rhs);
	Vector2_u8 & operator /= (u8 rhs);
	Vector2_u8 & operator %= (u8 rhs);

	Vector2_u8 operator + (Vector2_u8 rhs);
	Vector2_u8 operator - (Vector2_u8 rhs);
	Vector2_u8 operator * (Vector2_u8 rhs);
	Vector2_u8 operator / (Vector2_u8 rhs);
	Vector2_u8 operator % (Vector2_u8 rhs);

	Vector2_u8 operator + (u8 rhs);
	Vector2_u8 operator - (u8 rhs);
	Vector2_u8 operator * (u8 rhs);
	Vector2_u8 operator / (u8 rhs);
	Vector2_u8 operator % (u8 rhs);
};

struct Vector2_s16 {
	union { s16 x; s16 r; s16 width;  };
	union { s16 y; s16 g; s16 height; };

	bool operator == (Vector2_s16 rhs);
	bool operator == (s16 rhs);

	s16 & operator [] (int index);

	Vector2_s16 & operator += (Vector2_s16 rhs);
	Vector2_s16 & operator -= (Vector2_s16 rhs);
	Vector2_s16 & operator *= (Vector2_s16 rhs);
	Vector2_s16 & operator /= (Vector2_s16 rhs);
	Vector2_s16 & operator %= (Vector2_s16 rhs);

	Vector2_s16 & operator += (s16 rhs);
	Vector2_s16 & operator -= (s16 rhs);
	Vector2_s16 & operator *= (s16 rhs);
	Vector2_s16 & operator /= (s16 rhs);
	Vector2_s16 & operator %= (s16 rhs);

	Vector2_s16 operator + (Vector2_s16 rhs);
	Vector2_s16 operator - (Vector2_s16 rhs);
	Vector2_s16 operator * (Vector2_s16 rhs);
	Vector2_s16 operator / (Vector2_s16 rhs);
	Vector2_s16 operator % (Vector2_s16 rhs);

	Vector2_s16 operator + (s16 rhs);
	Vector2_s16 operator - (s16 rhs);
	Vector2_s16 operator * (s16 rhs);
	Vector2_s16 operator / (s16 rhs);
	Vector2_s16 operator % (s16 rhs);
};

struct Vector2_u16 {
	union { u16 x; u16 r; u16 width;  };
	union { u16 y; u16 g; u16 height; };

	bool operator == (Vector2_u16 rhs);
	bool operator == (u16 rhs);

	u16 & operator [] (int index);

	Vector2_u16 & operator += (Vector2_u16 rhs);
	Vector2_u16 & operator -= (Vector2_u16 rhs);
	Vector2_u16 & operator *= (Vector2_u16 rhs);
	Vector2_u16 & operator /= (Vector2_u16 rhs);
	Vector2_u16 & operator %= (Vector2_u16 rhs);

	Vector2_u16 & operator += (u16 rhs);
	Vector2_u16 & operator -= (u16 rhs);
	Vector2_u16 & operator *= (u16 rhs);
	Vector2_u16 & operator /= (u16 rhs);
	Vector2_u16 & operator %= (u16 rhs);

	Vector2_u16 operator + (Vector2_u16 rhs);
	Vector2_u16 operator - (Vector2_u16 rhs);
	Vector2_u16 operator * (Vector2_u16 rhs);
	Vector2_u16 operator / (Vector2_u16 rhs);
	Vector2_u16 operator % (Vector2_u16 rhs);

	Vector2_u16 operator + (u16 rhs);
	Vector2_u16 operator - (u16 rhs);
	Vector2_u16 operator * (u16 rhs);
	Vector2_u16 operator / (u16 rhs);
	Vector2_u16 operator % (u16 rhs);
};

struct Vector2_s32 {
	union { s32 x; s32 r; s32 width;  };
	union { s32 y; s32 g; s32 height; };

	bool operator == (Vector2_s32 rhs);
	bool operator == (s32 rhs);

	s32 & operator [] (int index);

	Vector2_s32 & operator += (Vector2_s32 rhs);
	Vector2_s32 & operator -= (Vector2_s32 rhs);
	Vector2_s32 & operator *= (Vector2_s32 rhs);
	Vector2_s32 & operator /= (Vector2_s32 rhs);
	Vector2_s32 & operator %= (Vector2_s32 rhs);

	Vector2_s32 & operator += (s32 rhs);
	Vector2_s32 & operator -= (s32 rhs);
	Vector2_s32 & operator *= (s32 rhs);
	Vector2_s32 & operator /= (s32 rhs);
	Vector2_s32 & operator %= (s32 rhs);

	Vector2_s32 operator + (Vector2_s32 rhs);
	Vector2_s32 operator - (Vector2_s32 rhs);
	Vector2_s32 operator * (Vector2_s32 rhs);
	Vector2_s32 operator / (Vector2_s32 rhs);
	Vector2_s32 operator % (Vector2_s32 rhs);

	Vector2_s32 operator + (s32 rhs);
	Vector2_s32 operator - (s32 rhs);
	Vector2_s32 operator * (s32 rhs);
	Vector2_s32 operator / (s32 rhs);
	Vector2_s32 operator % (s32 rhs);
};

struct Vector2_u32 {
	union { u32 x; u32 r; u32 width;  };
	union { u32 y; u32 g; u32 height; };

	bool operator == (Vector2_u32 rhs);
	bool operator == (u32 rhs);

	u32 & operator [] (int index);

	Vector2_u32 & operator += (Vector2_u32 rhs);
	Vector2_u32 & operator -= (Vector2_u32 rhs);
	Vector2_u32 & operator *= (Vector2_u32 rhs);
	Vector2_u32 & operator /= (Vector2_u32 rhs);
	Vector2_u32 & operator %= (Vector2_u32 rhs);

	Vector2_u32 & operator += (u32 rhs);
	Vector2_u32 & operator -= (u32 rhs);
	Vector2_u32 & operator *= (u32 rhs);
	Vector2_u32 & operator /= (u32 rhs);
	Vector2_u32 & operator %= (u32 rhs);

	Vector2_u32 operator + (Vector2_u32 rhs);
	Vector2_u32 operator - (Vector2_u32 rhs);
	Vector2_u32 operator * (Vector2_u32 rhs);
	Vector2_u32 operator / (Vector2_u32 rhs);
	Vector2_u32 operator % (Vector2_u32 rhs);

	Vector2_u32 operator + (u32 rhs);
	Vector2_u32 operator - (u32 rhs);
	Vector2_u32 operator * (u32 rhs);
	Vector2_u32 operator / (u32 rhs);
	Vector2_u32 operator % (u32 rhs);
};

struct Vector2_s64 {
	union { s64 x; s64 r; s64 width;  };
	union { s64 y; s64 g; s64 height; };

	bool operator == (Vector2_s64 rhs);
	bool operator == (s64 rhs);

	s64 & operator [] (int index);

	Vector2_s64 & operator += (Vector2_s64 rhs);
	Vector2_s64 & operator -= (Vector2_s64 rhs);
	Vector2_s64 & operator *= (Vector2_s64 rhs);
	Vector2_s64 & operator /= (Vector2_s64 rhs);
	Vector2_s64 & operator %= (Vector2_s64 rhs);

	Vector2_s64 & operator += (s64 rhs);
	Vector2_s64 & operator -= (s64 rhs);
	Vector2_s64 & operator *= (s64 rhs);
	Vector2_s64 & operator /= (s64 rhs);
	Vector2_s64 & operator %= (s64 rhs);

	Vector2_s64 operator + (Vector2_s64 rhs);
	Vector2_s64 operator - (Vector2_s64 rhs);
	Vector2_s64 operator * (Vector2_s64 rhs);
	Vector2_s64 operator / (Vector2_s64 rhs);
	Vector2_s64 operator % (Vector2_s64 rhs);

	Vector2_s64 operator + (s64 rhs);
	Vector2_s64 operator - (s64 rhs);
	Vector2_s64 operator * (s64 rhs);
	Vector2_s64 operator / (s64 rhs);
	Vector2_s64 operator % (s64 rhs);
};

struct Vector2_u64 {
	union { u64 x; u64 r; u64 width;  };
	union { u64 y; u64 g; u64 height; };

	bool operator == (Vector2_u64 rhs);
	bool operator == (u64 rhs);

	u64 & operator [] (int index);

	Vector2_u64 & operator += (Vector2_u64 rhs);
	Vector2_u64 & operator -= (Vector2_u64 rhs);
	Vector2_u64 & operator *= (Vector2_u64 rhs);
	Vector2_u64 & operator /= (Vector2_u64 rhs);
	Vector2_u64 & operator %= (Vector2_u64 rhs);

	Vector2_u64 & operator += (u64 rhs);
	Vector2_u64 & operator -= (u64 rhs);
	Vector2_u64 & operator *= (u64 rhs);
	Vector2_u64 & operator /= (u64 rhs);
	Vector2_u64 & operator %= (u64 rhs);

	Vector2_u64 operator + (Vector2_u64 rhs);
	Vector2_u64 operator - (Vector2_u64 rhs);
	Vector2_u64 operator * (Vector2_u64 rhs);
	Vector2_u64 operator / (Vector2_u64 rhs);
	Vector2_u64 operator % (Vector2_u64 rhs);

	Vector2_u64 operator + (u64 rhs);
	Vector2_u64 operator - (u64 rhs);
	Vector2_u64 operator * (u64 rhs);
	Vector2_u64 operator / (u64 rhs);
	Vector2_u64 operator % (u64 rhs);
};

struct Vector2_f32 {
	union { f32 x; f32 r; f32 width;  };
	union { f32 y; f32 g; f32 height; };

	bool operator == (Vector2_f32 rhs);
	bool operator == (f32 rhs);

	f32 & operator [] (int index);

	Vector2_f32 & operator += (Vector2_f32 rhs);
	Vector2_f32 & operator -= (Vector2_f32 rhs);
	Vector2_f32 & operator *= (Vector2_f32 rhs);
	Vector2_f32 & operator /= (Vector2_f32 rhs);

	Vector2_f32 & operator += (f32 rhs);
	Vector2_f32 & operator -= (f32 rhs);
	Vector2_f32 & operator *= (f32 rhs);
	Vector2_f32 & operator /= (f32 rhs);

	Vector2_f32 operator + (Vector2_f32 rhs);
	Vector2_f32 operator - (Vector2_f32 rhs);
	Vector2_f32 operator * (Vector2_f32 rhs);
	Vector2_f32 operator / (Vector2_f32 rhs);

	Vector2_f32 operator + (f32 rhs);
	Vector2_f32 operator - (f32 rhs);
	Vector2_f32 operator * (f32 rhs);
	Vector2_f32 operator / (f32 rhs);
};

struct Vector2_f64 {
	union { f64 x; f64 r; f64 width;  };
	union { f64 y; f64 g; f64 height; };

	bool operator == (Vector2_f64 rhs);
	bool operator == (f64 rhs);

	f64 & operator [] (int index);

	Vector2_f64 & operator += (Vector2_f64 rhs);
	Vector2_f64 & operator -= (Vector2_f64 rhs);
	Vector2_f64 & operator *= (Vector2_f64 rhs);
	Vector2_f64 & operator /= (Vector2_f64 rhs);

	Vector2_f64 & operator += (f64 rhs);
	Vector2_f64 & operator -= (f64 rhs);
	Vector2_f64 & operator *= (f64 rhs);
	Vector2_f64 & operator /= (f64 rhs);

	Vector2_f64 operator + (Vector2_f64 rhs);
	Vector2_f64 operator - (Vector2_f64 rhs);
	Vector2_f64 operator * (Vector2_f64 rhs);
	Vector2_f64 operator / (Vector2_f64 rhs);

	Vector2_f64 operator + (f64 rhs);
	Vector2_f64 operator - (f64 rhs);
	Vector2_f64 operator * (f64 rhs);
	Vector2_f64 operator / (f64 rhs);
};

//-----------------------------------------------------------------------------
// [SECTION] Vector3: s8 -> u8 -> s16 -> u16 -> s32 -> u32 -> s64 -> u64 -> f32 -> f64
//-----------------------------------------------------------------------------

struct Vector3_s8 {
	union { s8 x; s8 r; s8 width;  };
	union { s8 y; s8 g; s8 height; };
	union { s8 z; s8 b; s8 depth;  };

	bool operator == (Vector3_s8 rhs);
	bool operator == (s8 rhs);

	s8 & operator [] (int index);

	Vector3_s8 & operator += (Vector3_s8 rhs);
	Vector3_s8 & operator -= (Vector3_s8 rhs);
	Vector3_s8 & operator *= (Vector3_s8 rhs);
	Vector3_s8 & operator /= (Vector3_s8 rhs);
	Vector3_s8 & operator %= (Vector3_s8 rhs);

	Vector3_s8 & operator += (s8 rhs);
	Vector3_s8 & operator -= (s8 rhs);
	Vector3_s8 & operator *= (s8 rhs);
	Vector3_s8 & operator /= (s8 rhs);
	Vector3_s8 & operator %= (s8 rhs);

	Vector3_s8 operator + (Vector3_s8 rhs);
	Vector3_s8 operator - (Vector3_s8 rhs);
	Vector3_s8 operator * (Vector3_s8 rhs);
	Vector3_s8 operator / (Vector3_s8 rhs);
	Vector3_s8 operator % (Vector3_s8 rhs);

	Vector3_s8 operator + (s8 rhs);
	Vector3_s8 operator - (s8 rhs);
	Vector3_s8 operator * (s8 rhs);
	Vector3_s8 operator / (s8 rhs);
	Vector3_s8 operator % (s8 rhs);
};

struct Vector3_u8 {
	union { u8 x; u8 r; u8 width;  };
	union { u8 y; u8 g; u8 height; };
	union { u8 z; u8 b; u8 depth;  };

	bool operator == (Vector3_u8 rhs);
	bool operator == (u8 rhs);

	u8 & operator [] (int index);

	Vector3_u8 & operator += (Vector3_u8 rhs);
	Vector3_u8 & operator -= (Vector3_u8 rhs);
	Vector3_u8 & operator *= (Vector3_u8 rhs);
	Vector3_u8 & operator /= (Vector3_u8 rhs);
	Vector3_u8 & operator %= (Vector3_u8 rhs);

	Vector3_u8 & operator += (u8 rhs);
	Vector3_u8 & operator -= (u8 rhs);
	Vector3_u8 & operator *= (u8 rhs);
	Vector3_u8 & operator /= (u8 rhs);
	Vector3_u8 & operator %= (u8 rhs);

	Vector3_u8 operator + (Vector3_u8 rhs);
	Vector3_u8 operator - (Vector3_u8 rhs);
	Vector3_u8 operator * (Vector3_u8 rhs);
	Vector3_u8 operator / (Vector3_u8 rhs);
	Vector3_u8 operator % (Vector3_u8 rhs);

	Vector3_u8 operator + (u8 rhs);
	Vector3_u8 operator - (u8 rhs);
	Vector3_u8 operator * (u8 rhs);
	Vector3_u8 operator / (u8 rhs);
	Vector3_u8 operator % (u8 rhs);
};

struct Vector3_s16 {
	union { s16 x; s16 r; s16 width;  };
	union { s16 y; s16 g; s16 height; };
	union { s16 z; s16 b; s16 depth;  };

	bool operator == (Vector3_s16 rhs);
	bool operator == (s16 rhs);

	s16 & operator [] (int index);

	Vector3_s16 & operator += (Vector3_s16 rhs);
	Vector3_s16 & operator -= (Vector3_s16 rhs);
	Vector3_s16 & operator *= (Vector3_s16 rhs);
	Vector3_s16 & operator /= (Vector3_s16 rhs);
	Vector3_s16 & operator %= (Vector3_s16 rhs);

	Vector3_s16 & operator += (s16 rhs);
	Vector3_s16 & operator -= (s16 rhs);
	Vector3_s16 & operator *= (s16 rhs);
	Vector3_s16 & operator /= (s16 rhs);
	Vector3_s16 & operator %= (s16 rhs);

	Vector3_s16 operator + (Vector3_s16 rhs);
	Vector3_s16 operator - (Vector3_s16 rhs);
	Vector3_s16 operator * (Vector3_s16 rhs);
	Vector3_s16 operator / (Vector3_s16 rhs);
	Vector3_s16 operator % (Vector3_s16 rhs);

	Vector3_s16 operator + (s16 rhs);
	Vector3_s16 operator - (s16 rhs);
	Vector3_s16 operator * (s16 rhs);
	Vector3_s16 operator / (s16 rhs);
	Vector3_s16 operator % (s16 rhs);
};

struct Vector3_u16 {
	union { u16 x; u16 r; u16 width;  };
	union { u16 y; u16 g; u16 height; };
	union { u16 z; u16 b; u16 depth;  };

	bool operator == (Vector3_u16 rhs);
	bool operator == (u16 rhs);

	u16 & operator [] (int index);

	Vector3_u16 & operator += (Vector3_u16 rhs);
	Vector3_u16 & operator -= (Vector3_u16 rhs);
	Vector3_u16 & operator *= (Vector3_u16 rhs);
	Vector3_u16 & operator /= (Vector3_u16 rhs);
	Vector3_u16 & operator %= (Vector3_u16 rhs);

	Vector3_u16 & operator += (u16 rhs);
	Vector3_u16 & operator -= (u16 rhs);
	Vector3_u16 & operator *= (u16 rhs);
	Vector3_u16 & operator /= (u16 rhs);
	Vector3_u16 & operator %= (u16 rhs);

	Vector3_u16 operator + (Vector3_u16 rhs);
	Vector3_u16 operator - (Vector3_u16 rhs);
	Vector3_u16 operator * (Vector3_u16 rhs);
	Vector3_u16 operator / (Vector3_u16 rhs);
	Vector3_u16 operator % (Vector3_u16 rhs);

	Vector3_u16 operator + (u16 rhs);
	Vector3_u16 operator - (u16 rhs);
	Vector3_u16 operator * (u16 rhs);
	Vector3_u16 operator / (u16 rhs);
	Vector3_u16 operator % (u16 rhs);
};

struct Vector3_s32 {
	union { s32 x; s32 r; s32 width;  };
	union { s32 y; s32 g; s32 height; };
	union { s32 z; s32 b; s32 depth;  };

	bool operator == (Vector3_s32 rhs);
	bool operator == (s32 rhs);

	s32 & operator [] (int index);

	Vector3_s32 & operator += (Vector3_s32 rhs);
	Vector3_s32 & operator -= (Vector3_s32 rhs);
	Vector3_s32 & operator *= (Vector3_s32 rhs);
	Vector3_s32 & operator /= (Vector3_s32 rhs);
	Vector3_s32 & operator %= (Vector3_s32 rhs);

	Vector3_s32 & operator += (s32 rhs);
	Vector3_s32 & operator -= (s32 rhs);
	Vector3_s32 & operator *= (s32 rhs);
	Vector3_s32 & operator /= (s32 rhs);
	Vector3_s32 & operator %= (s32 rhs);

	Vector3_s32 operator + (Vector3_s32 rhs);
	Vector3_s32 operator - (Vector3_s32 rhs);
	Vector3_s32 operator * (Vector3_s32 rhs);
	Vector3_s32 operator / (Vector3_s32 rhs);
	Vector3_s32 operator % (Vector3_s32 rhs);

	Vector3_s32 operator + (s32 rhs);
	Vector3_s32 operator - (s32 rhs);
	Vector3_s32 operator * (s32 rhs);
	Vector3_s32 operator / (s32 rhs);
	Vector3_s32 operator % (s32 rhs);
};

struct Vector3_u32 {
	union { u32 x; u32 r; u32 width;  };
	union { u32 y; u32 g; u32 height; };
	union { u32 z; u32 b; u32 depth;  };

	bool operator == (Vector3_u32 rhs);
	bool operator == (u32 rhs);

	u32 & operator [] (int index);

	Vector3_u32 & operator += (Vector3_u32 rhs);
	Vector3_u32 & operator -= (Vector3_u32 rhs);
	Vector3_u32 & operator *= (Vector3_u32 rhs);
	Vector3_u32 & operator /= (Vector3_u32 rhs);
	Vector3_u32 & operator %= (Vector3_u32 rhs);

	Vector3_u32 & operator += (u32 rhs);
	Vector3_u32 & operator -= (u32 rhs);
	Vector3_u32 & operator *= (u32 rhs);
	Vector3_u32 & operator /= (u32 rhs);
	Vector3_u32 & operator %= (u32 rhs);

	Vector3_u32 operator + (Vector3_u32 rhs);
	Vector3_u32 operator - (Vector3_u32 rhs);
	Vector3_u32 operator * (Vector3_u32 rhs);
	Vector3_u32 operator / (Vector3_u32 rhs);
	Vector3_u32 operator % (Vector3_u32 rhs);

	Vector3_u32 operator + (u32 rhs);
	Vector3_u32 operator - (u32 rhs);
	Vector3_u32 operator * (u32 rhs);
	Vector3_u32 operator / (u32 rhs);
	Vector3_u32 operator % (u32 rhs);
};

struct Vector3_s64 {
	union { s64 x; s64 r; s64 width;  };
	union { s64 y; s64 g; s64 height; };
	union { s64 z; s64 b; s64 depth;  };

	bool operator == (Vector3_s64 rhs);
	bool operator == (s64 rhs);

	s64 & operator [] (int index);

	Vector3_s64 & operator += (Vector3_s64 rhs);
	Vector3_s64 & operator -= (Vector3_s64 rhs);
	Vector3_s64 & operator *= (Vector3_s64 rhs);
	Vector3_s64 & operator /= (Vector3_s64 rhs);
	Vector3_s64 & operator %= (Vector3_s64 rhs);

	Vector3_s64 & operator += (s64 rhs);
	Vector3_s64 & operator -= (s64 rhs);
	Vector3_s64 & operator *= (s64 rhs);
	Vector3_s64 & operator /= (s64 rhs);
	Vector3_s64 & operator %= (s64 rhs);

	Vector3_s64 operator + (Vector3_s64 rhs);
	Vector3_s64 operator - (Vector3_s64 rhs);
	Vector3_s64 operator * (Vector3_s64 rhs);
	Vector3_s64 operator / (Vector3_s64 rhs);
	Vector3_s64 operator % (Vector3_s64 rhs);

	Vector3_s64 operator + (s64 rhs);
	Vector3_s64 operator - (s64 rhs);
	Vector3_s64 operator * (s64 rhs);
	Vector3_s64 operator / (s64 rhs);
	Vector3_s64 operator % (s64 rhs);
};

struct Vector3_u64 {
	union { u64 x; u64 r; u64 width;  };
	union { u64 y; u64 g; u64 height; };
	union { u64 z; u64 b; u64 depth;  };

	bool operator == (Vector3_u64 rhs);
	bool operator == (u64 rhs);

	u64 & operator [] (int index);

	Vector3_u64 & operator += (Vector3_u64 rhs);
	Vector3_u64 & operator -= (Vector3_u64 rhs);
	Vector3_u64 & operator *= (Vector3_u64 rhs);
	Vector3_u64 & operator /= (Vector3_u64 rhs);
	Vector3_u64 & operator %= (Vector3_u64 rhs);

	Vector3_u64 & operator += (u64 rhs);
	Vector3_u64 & operator -= (u64 rhs);
	Vector3_u64 & operator *= (u64 rhs);
	Vector3_u64 & operator /= (u64 rhs);
	Vector3_u64 & operator %= (u64 rhs);

	Vector3_u64 operator + (Vector3_u64 rhs);
	Vector3_u64 operator - (Vector3_u64 rhs);
	Vector3_u64 operator * (Vector3_u64 rhs);
	Vector3_u64 operator / (Vector3_u64 rhs);
	Vector3_u64 operator % (Vector3_u64 rhs);

	Vector3_u64 operator + (u64 rhs);
	Vector3_u64 operator - (u64 rhs);
	Vector3_u64 operator * (u64 rhs);
	Vector3_u64 operator / (u64 rhs);
	Vector3_u64 operator % (u64 rhs);
};

struct Vector3_f32 {
	union { f32 x; f32 r; f32 width;  };
	union { f32 y; f32 g; f32 height; };
	union { f32 z; f32 b; f32 depth;  };

	bool operator == (Vector3_f32 rhs);
	bool operator == (f32 rhs);

	f32 & operator [] (int index);

	Vector3_f32 & operator += (Vector3_f32 rhs);
	Vector3_f32 & operator -= (Vector3_f32 rhs);
	Vector3_f32 & operator *= (Vector3_f32 rhs);
	Vector3_f32 & operator /= (Vector3_f32 rhs);

	Vector3_f32 & operator += (f32 rhs);
	Vector3_f32 & operator -= (f32 rhs);
	Vector3_f32 & operator *= (f32 rhs);
	Vector3_f32 & operator /= (f32 rhs);

	Vector3_f32 operator + (Vector3_f32 rhs);
	Vector3_f32 operator - (Vector3_f32 rhs);
	Vector3_f32 operator * (Vector3_f32 rhs);
	Vector3_f32 operator / (Vector3_f32 rhs);

	Vector3_f32 operator + (f32 rhs);
	Vector3_f32 operator - (f32 rhs);
	Vector3_f32 operator * (f32 rhs);
	Vector3_f32 operator / (f32 rhs);

	Vector3_f32 operator - ();
	Vector3_f32 operator - () const;
};

struct Vector3_f64 {
	union { f64 x; f64 r; f64 width;  };
	union { f64 y; f64 g; f64 height; };
	union { f64 z; f64 b; f64 depth;  };

	bool operator == (Vector3_f64 rhs);
	bool operator == (f64 rhs);

	f64 & operator [] (int index);

	Vector3_f64 & operator += (Vector3_f64 rhs);
	Vector3_f64 & operator -= (Vector3_f64 rhs);
	Vector3_f64 & operator *= (Vector3_f64 rhs);
	Vector3_f64 & operator /= (Vector3_f64 rhs);

	Vector3_f64 & operator += (f64 rhs);
	Vector3_f64 & operator -= (f64 rhs);
	Vector3_f64 & operator *= (f64 rhs);
	Vector3_f64 & operator /= (f64 rhs);

	Vector3_f64 operator + (Vector3_f64 rhs);
	Vector3_f64 operator - (Vector3_f64 rhs);
	Vector3_f64 operator * (Vector3_f64 rhs);
	Vector3_f64 operator / (Vector3_f64 rhs);

	Vector3_f64 operator + (f64 rhs);
	Vector3_f64 operator - (f64 rhs);
	Vector3_f64 operator * (f64 rhs);
	Vector3_f64 operator / (f64 rhs);
};

//-----------------------------------------------------------------------------
// [SECTION] Vector4: s8 -> u8 -> s16 -> u16 -> s32 -> u32 -> s64 -> u64 -> f32 -> f64
//-----------------------------------------------------------------------------

struct Vector4_s8 {
	union { s8 x; s8 r; s8 width;  };
	union { s8 y; s8 g; s8 height; };
	union { s8 z; s8 b; s8 depth;  };
	union { s8 w; s8 a;            };

	bool operator == (Vector4_s8 rhs);
	bool operator == (s8 rhs);

	s8 & operator [] (int index);

	Vector4_s8 & operator += (Vector4_s8 rhs);
	Vector4_s8 & operator -= (Vector4_s8 rhs);
	Vector4_s8 & operator *= (Vector4_s8 rhs);
	Vector4_s8 & operator /= (Vector4_s8 rhs);
	Vector4_s8 & operator %= (Vector4_s8 rhs);

	Vector4_s8 & operator += (s8 rhs);
	Vector4_s8 & operator -= (s8 rhs);
	Vector4_s8 & operator *= (s8 rhs);
	Vector4_s8 & operator /= (s8 rhs);
	Vector4_s8 & operator %= (s8 rhs);

	Vector4_s8 operator + (Vector4_s8 rhs);
	Vector4_s8 operator - (Vector4_s8 rhs);
	Vector4_s8 operator * (Vector4_s8 rhs);
	Vector4_s8 operator / (Vector4_s8 rhs);
	Vector4_s8 operator % (Vector4_s8 rhs);

	Vector4_s8 operator + (s8 rhs);
	Vector4_s8 operator - (s8 rhs);
	Vector4_s8 operator * (s8 rhs);
	Vector4_s8 operator / (s8 rhs);
	Vector4_s8 operator % (s8 rhs);
};

struct Vector4_u8 {
	union { u8 x; u8 r; u8 width;  };
	union { u8 y; u8 g; u8 height; };
	union { u8 z; u8 b; u8 depth;  };
	union { u8 w; u8 a;            };

	bool operator == (Vector4_u8 rhs);
	bool operator == (u8 rhs);

	u8 & operator [] (int index);

	Vector4_u8 & operator += (Vector4_u8 rhs);
	Vector4_u8 & operator -= (Vector4_u8 rhs);
	Vector4_u8 & operator *= (Vector4_u8 rhs);
	Vector4_u8 & operator /= (Vector4_u8 rhs);
	Vector4_u8 & operator %= (Vector4_u8 rhs);

	Vector4_u8 & operator += (u8 rhs);
	Vector4_u8 & operator -= (u8 rhs);
	Vector4_u8 & operator *= (u8 rhs);
	Vector4_u8 & operator /= (u8 rhs);
	Vector4_u8 & operator %= (u8 rhs);

	Vector4_u8 operator + (Vector4_u8 rhs);
	Vector4_u8 operator - (Vector4_u8 rhs);
	Vector4_u8 operator * (Vector4_u8 rhs);
	Vector4_u8 operator / (Vector4_u8 rhs);
	Vector4_u8 operator % (Vector4_u8 rhs);

	Vector4_u8 operator + (u8 rhs);
	Vector4_u8 operator - (u8 rhs);
	Vector4_u8 operator * (u8 rhs);
	Vector4_u8 operator / (u8 rhs);
	Vector4_u8 operator % (u8 rhs);
};

struct Vector4_s16 {
	union { s16 x; s16 r; s16 width;  };
	union { s16 y; s16 g; s16 height; };
	union { s16 z; s16 b; s16 depth;  };
	union { s16 w; s16 a;             };

	bool operator == (Vector4_s16 rhs);
	bool operator == (s16 rhs);

	s16 & operator [] (int index);

	Vector4_s16 & operator += (Vector4_s16 rhs);
	Vector4_s16 & operator -= (Vector4_s16 rhs);
	Vector4_s16 & operator *= (Vector4_s16 rhs);
	Vector4_s16 & operator /= (Vector4_s16 rhs);
	Vector4_s16 & operator %= (Vector4_s16 rhs);

	Vector4_s16 & operator += (s16 rhs);
	Vector4_s16 & operator -= (s16 rhs);
	Vector4_s16 & operator *= (s16 rhs);
	Vector4_s16 & operator /= (s16 rhs);
	Vector4_s16 & operator %= (s16 rhs);

	Vector4_s16 operator + (Vector4_s16 rhs);
	Vector4_s16 operator - (Vector4_s16 rhs);
	Vector4_s16 operator * (Vector4_s16 rhs);
	Vector4_s16 operator / (Vector4_s16 rhs);
	Vector4_s16 operator % (Vector4_s16 rhs);

	Vector4_s16 operator + (s16 rhs);
	Vector4_s16 operator - (s16 rhs);
	Vector4_s16 operator * (s16 rhs);
	Vector4_s16 operator / (s16 rhs);
	Vector4_s16 operator % (s16 rhs);
};

struct Vector4_u16 {
	union { u16 x; u16 r; u16 width;  };
	union { u16 y; u16 g; u16 height; };
	union { u16 z; u16 b; u16 depth;  };
	union { u16 w; u16 a;             };

	bool operator == (Vector4_u16 rhs);
	bool operator == (u16 rhs);

	u16 & operator [] (int index);

	Vector4_u16 & operator += (Vector4_u16 rhs);
	Vector4_u16 & operator -= (Vector4_u16 rhs);
	Vector4_u16 & operator *= (Vector4_u16 rhs);
	Vector4_u16 & operator /= (Vector4_u16 rhs);
	Vector4_u16 & operator %= (Vector4_u16 rhs);

	Vector4_u16 & operator += (u16 rhs);
	Vector4_u16 & operator -= (u16 rhs);
	Vector4_u16 & operator *= (u16 rhs);
	Vector4_u16 & operator /= (u16 rhs);
	Vector4_u16 & operator %= (u16 rhs);

	Vector4_u16 operator + (Vector4_u16 rhs);
	Vector4_u16 operator - (Vector4_u16 rhs);
	Vector4_u16 operator * (Vector4_u16 rhs);
	Vector4_u16 operator / (Vector4_u16 rhs);
	Vector4_u16 operator % (Vector4_u16 rhs);

	Vector4_u16 operator + (u16 rhs);
	Vector4_u16 operator - (u16 rhs);
	Vector4_u16 operator * (u16 rhs);
	Vector4_u16 operator / (u16 rhs);
	Vector4_u16 operator % (u16 rhs);
};

struct Vector4_s32 {
	union { s32 x; s32 r; s32 width;  };
	union { s32 y; s32 g; s32 height; };
	union { s32 z; s32 b; s32 depth;  };
	union { s32 w; s32 a;             };

	bool operator == (Vector4_s32 rhs);
	bool operator == (s32 rhs);

	s32 & operator [] (int index);

	Vector4_s32 & operator += (Vector4_s32 rhs);
	Vector4_s32 & operator -= (Vector4_s32 rhs);
	Vector4_s32 & operator *= (Vector4_s32 rhs);
	Vector4_s32 & operator /= (Vector4_s32 rhs);
	Vector4_s32 & operator %= (Vector4_s32 rhs);

	Vector4_s32 & operator += (s32 rhs);
	Vector4_s32 & operator -= (s32 rhs);
	Vector4_s32 & operator *= (s32 rhs);
	Vector4_s32 & operator /= (s32 rhs);
	Vector4_s32 & operator %= (s32 rhs);

	Vector4_s32 operator + (Vector4_s32 rhs);
	Vector4_s32 operator - (Vector4_s32 rhs);
	Vector4_s32 operator * (Vector4_s32 rhs);
	Vector4_s32 operator / (Vector4_s32 rhs);
	Vector4_s32 operator % (Vector4_s32 rhs);

	Vector4_s32 operator + (s32 rhs);
	Vector4_s32 operator - (s32 rhs);
	Vector4_s32 operator * (s32 rhs);
	Vector4_s32 operator / (s32 rhs);
	Vector4_s32 operator % (s32 rhs);
};

struct Vector4_u32 {
	union { u32 x; u32 r; u32 width;  };
	union { u32 y; u32 g; u32 height; };
	union { u32 z; u32 b; u32 depth;  };
	union { u32 w; u32 a;             };

	bool operator == (Vector4_u32 rhs);
	bool operator == (u32 rhs);

	u32 & operator [] (int index);

	Vector4_u32 & operator += (Vector4_u32 rhs);
	Vector4_u32 & operator -= (Vector4_u32 rhs);
	Vector4_u32 & operator *= (Vector4_u32 rhs);
	Vector4_u32 & operator /= (Vector4_u32 rhs);
	Vector4_u32 & operator %= (Vector4_u32 rhs);

	Vector4_u32 & operator += (u32 rhs);
	Vector4_u32 & operator -= (u32 rhs);
	Vector4_u32 & operator *= (u32 rhs);
	Vector4_u32 & operator /= (u32 rhs);
	Vector4_u32 & operator %= (u32 rhs);

	Vector4_u32 operator + (Vector4_u32 rhs);
	Vector4_u32 operator - (Vector4_u32 rhs);
	Vector4_u32 operator * (Vector4_u32 rhs);
	Vector4_u32 operator / (Vector4_u32 rhs);
	Vector4_u32 operator % (Vector4_u32 rhs);

	Vector4_u32 operator + (u32 rhs);
	Vector4_u32 operator - (u32 rhs);
	Vector4_u32 operator * (u32 rhs);
	Vector4_u32 operator / (u32 rhs);
	Vector4_u32 operator % (u32 rhs);
};

struct Vector4_s64 {
	union { s64 x; s64 r; s64 width;  };
	union { s64 y; s64 g; s64 height; };
	union { s64 z; s64 b; s64 depth;  };
	union { s64 w; s64 a;             };

	bool operator == (Vector4_s64 rhs);
	bool operator == (s64 rhs);

	s64 & operator [] (int index);

	Vector4_s64 & operator += (Vector4_s64 rhs);
	Vector4_s64 & operator -= (Vector4_s64 rhs);
	Vector4_s64 & operator *= (Vector4_s64 rhs);
	Vector4_s64 & operator /= (Vector4_s64 rhs);
	Vector4_s64 & operator %= (Vector4_s64 rhs);

	Vector4_s64 & operator += (s64 rhs);
	Vector4_s64 & operator -= (s64 rhs);
	Vector4_s64 & operator *= (s64 rhs);
	Vector4_s64 & operator /= (s64 rhs);
	Vector4_s64 & operator %= (s64 rhs);

	Vector4_s64 operator + (Vector4_s64 rhs);
	Vector4_s64 operator - (Vector4_s64 rhs);
	Vector4_s64 operator * (Vector4_s64 rhs);
	Vector4_s64 operator / (Vector4_s64 rhs);
	Vector4_s64 operator % (Vector4_s64 rhs);

	Vector4_s64 operator + (s64 rhs);
	Vector4_s64 operator - (s64 rhs);
	Vector4_s64 operator * (s64 rhs);
	Vector4_s64 operator / (s64 rhs);
	Vector4_s64 operator % (s64 rhs);
};

struct Vector4_u64 {
	union { u64 x; u64 r; u64 width;  };
	union { u64 y; u64 g; u64 height; };
	union { u64 z; u64 b; u64 depth;  };
	union { u64 w; u64 a;             };

	bool operator == (Vector4_u64 rhs);
	bool operator == (u64 rhs);

	u64 & operator [] (int index);

	Vector4_u64 & operator += (Vector4_u64 rhs);
	Vector4_u64 & operator -= (Vector4_u64 rhs);
	Vector4_u64 & operator *= (Vector4_u64 rhs);
	Vector4_u64 & operator /= (Vector4_u64 rhs);
	Vector4_u64 & operator %= (Vector4_u64 rhs);

	Vector4_u64 & operator += (u64 rhs);
	Vector4_u64 & operator -= (u64 rhs);
	Vector4_u64 & operator *= (u64 rhs);
	Vector4_u64 & operator /= (u64 rhs);
	Vector4_u64 & operator %= (u64 rhs);

	Vector4_u64 operator + (Vector4_u64 rhs);
	Vector4_u64 operator - (Vector4_u64 rhs);
	Vector4_u64 operator * (Vector4_u64 rhs);
	Vector4_u64 operator / (Vector4_u64 rhs);
	Vector4_u64 operator % (Vector4_u64 rhs);

	Vector4_u64 operator + (u64 rhs);
	Vector4_u64 operator - (u64 rhs);
	Vector4_u64 operator * (u64 rhs);
	Vector4_u64 operator / (u64 rhs);
	Vector4_u64 operator % (u64 rhs);
};

struct Vector4_f32 {
	union { f32 x; f32 r; f32 width;  };
	union { f32 y; f32 g; f32 height; };
	union { f32 z; f32 b; f32 depth;  };
	union { f32 w; f32 a;             };

	bool operator == (Vector4_f32 rhs);
	bool operator == (f32 rhs);

	f32 & operator [] (int index);

	Vector4_f32 & operator += (Vector4_f32 rhs);
	Vector4_f32 & operator -= (Vector4_f32 rhs);
	Vector4_f32 & operator *= (Vector4_f32 rhs);
	Vector4_f32 & operator /= (Vector4_f32 rhs);

	Vector4_f32 & operator += (f32 rhs);
	Vector4_f32 & operator -= (f32 rhs);
	Vector4_f32 & operator *= (f32 rhs);
	Vector4_f32 & operator /= (f32 rhs);

	Vector4_f32 operator + (Vector4_f32 rhs);
	Vector4_f32 operator - (Vector4_f32 rhs);
	Vector4_f32 operator * (Vector4_f32 rhs);
	Vector4_f32 operator / (Vector4_f32 rhs);

	Vector4_f32 operator + (f32 rhs);
	Vector4_f32 operator - (f32 rhs);
	Vector4_f32 operator * (f32 rhs);
	Vector4_f32 operator / (f32 rhs);
};

struct Vector4_f64 {
	union { f64 x; f64 r; f64 width;  };
	union { f64 y; f64 g; f64 height; };
	union { f64 z; f64 b; f64 depth;  };
	union { f64 w; f64 a;             };

	bool operator == (Vector4_f64 rhs);
	bool operator == (f64 rhs);

	f64 & operator [] (int index);

	Vector4_f64 & operator += (Vector4_f64 rhs);
	Vector4_f64 & operator -= (Vector4_f64 rhs);
	Vector4_f64 & operator *= (Vector4_f64 rhs);
	Vector4_f64 & operator /= (Vector4_f64 rhs);

	Vector4_f64 & operator += (f64 rhs);
	Vector4_f64 & operator -= (f64 rhs);
	Vector4_f64 & operator *= (f64 rhs);
	Vector4_f64 & operator /= (f64 rhs);

	Vector4_f64 operator + (Vector4_f64 rhs);
	Vector4_f64 operator - (Vector4_f64 rhs);
	Vector4_f64 operator * (Vector4_f64 rhs);
	Vector4_f64 operator / (Vector4_f64 rhs);

	Vector4_f64 operator + (f64 rhs);
	Vector4_f64 operator - (f64 rhs);
	Vector4_f64 operator * (f64 rhs);
	Vector4_f64 operator / (f64 rhs);
};

//-----------------------------------------------------------------------------
// [SECTION] Matrix3x3: f32 -> f64
//-----------------------------------------------------------------------------

struct Matrix3x3_f32 {
	Vector3_f32 columns[3];

	Matrix3x3_f32();
	Matrix3x3_f32(f32 value);
	Matrix3x3_f32(Vector3_f32 column0, Vector3_f32 column1, Vector3_f32 column2);

	bool operator == (Matrix3x3_f32 rhs);
	bool operator == (f32 rhs);

	Vector3_f32 & operator [] (int index);

	Matrix3x3_f32 & operator += (Matrix3x3_f32 rhs);
	Matrix3x3_f32 & operator -= (Matrix3x3_f32 rhs);
	Matrix3x3_f32 & operator *= (Matrix3x3_f32 rhs);
	Matrix3x3_f32 & operator /= (Matrix3x3_f32 rhs);

	Matrix3x3_f32 & operator += (f32 rhs);
	Matrix3x3_f32 & operator -= (f32 rhs);
	Matrix3x3_f32 & operator *= (f32 rhs);
	Matrix3x3_f32 & operator /= (f32 rhs);

	Matrix3x3_f32 operator + (Matrix3x3_f32 rhs);
	Matrix3x3_f32 operator - (Matrix3x3_f32 rhs);
	Matrix3x3_f32 operator * (Matrix3x3_f32 rhs);
	Matrix3x3_f32 operator / (Matrix3x3_f32 rhs);

	Matrix3x3_f32 operator + (f32 rhs);
	Matrix3x3_f32 operator - (f32 rhs);
	Matrix3x3_f32 operator * (f32 rhs);
	Matrix3x3_f32 operator / (f32 rhs);
};

Matrix3x3_f32  matrix3x3_f32_transpose(Matrix3x3_f32 matrix);
          f32  matrix3x3_f32_determinant(Matrix3x3_f32 matrix);
Matrix3x3_f32  matrix3x3_f32_inverse(Matrix3x3_f32 matrix);
Matrix3x3_f32  matrix3x3_f32_multiply(Matrix3x3_f32 lhs, Matrix3x3_f32 rhs);

struct Matrix3x3_f64 {
	Vector3_f64 columns[3];

	Matrix3x3_f64();
	Matrix3x3_f64(f64 value);
	Matrix3x3_f64(Vector3_f64 column0, Vector3_f64 column1, Vector3_f64 column2);

	bool operator == (Matrix3x3_f64 rhs);
	bool operator == (f64 rhs);

	Vector3_f64 & operator [] (int index);

	Matrix3x3_f64 & operator += (Matrix3x3_f64 rhs);
	Matrix3x3_f64 & operator -= (Matrix3x3_f64 rhs);
	Matrix3x3_f64 & operator *= (Matrix3x3_f64 rhs);
	Matrix3x3_f64 & operator /= (Matrix3x3_f64 rhs);

	Matrix3x3_f64 & operator += (f64 rhs);
	Matrix3x3_f64 & operator -= (f64 rhs);
	Matrix3x3_f64 & operator *= (f64 rhs);
	Matrix3x3_f64 & operator /= (f64 rhs);

	Matrix3x3_f64 operator + (Matrix3x3_f64 rhs);
	Matrix3x3_f64 operator - (Matrix3x3_f64 rhs);
	Matrix3x3_f64 operator * (Matrix3x3_f64 rhs);
	Matrix3x3_f64 operator / (Matrix3x3_f64 rhs);

	Matrix3x3_f64 operator + (f64 rhs);
	Matrix3x3_f64 operator - (f64 rhs);
	Matrix3x3_f64 operator * (f64 rhs);
	Matrix3x3_f64 operator / (f64 rhs);
};

Matrix3x3_f64  matrix3x3_f64_transpose(Matrix3x3_f64 matrix);
          f64  matrix3x3_f64_determinant(Matrix3x3_f64 matrix);
Matrix3x3_f64  matrix3x3_f64_inverse(Matrix3x3_f64 matrix);
Matrix3x3_f64  matrix3x3_f64_multiply(Matrix3x3_f64 lhs, Matrix3x3_f64 rhs);

//-----------------------------------------------------------------------------
// [SECTION] Matrix4x4: f32 -> f64
//-----------------------------------------------------------------------------

struct Matrix4x4_f32 {
	Vector4_f32 columns[4];

	Matrix4x4_f32();
	Matrix4x4_f32(f32 value);
	Matrix4x4_f32(Vector4_f32 column0, Vector4_f32 column1, Vector4_f32 column2, Vector4_f32 column3);

	bool operator == (Matrix4x4_f32 rhs);
	bool operator == (f32 rhs);

	Vector4_f32 & operator [] (int index);

	Matrix4x4_f32 & operator += (Matrix4x4_f32 rhs);
	Matrix4x4_f32 & operator -= (Matrix4x4_f32 rhs);
	Matrix4x4_f32 & operator *= (Matrix4x4_f32 rhs);
	Matrix4x4_f32 & operator /= (Matrix4x4_f32 rhs);

	Matrix4x4_f32 & operator += (f32 rhs);
	Matrix4x4_f32 & operator -= (f32 rhs);
	Matrix4x4_f32 & operator *= (f32 rhs);
	Matrix4x4_f32 & operator /= (f32 rhs);

	Matrix4x4_f32 operator + (Matrix4x4_f32 rhs);
	Matrix4x4_f32 operator - (Matrix4x4_f32 rhs);
	Matrix4x4_f32 operator * (Matrix4x4_f32 rhs);
	Matrix4x4_f32 operator / (Matrix4x4_f32 rhs);

	Matrix4x4_f32 operator + (f32 rhs);
	Matrix4x4_f32 operator - (f32 rhs);
	Matrix4x4_f32 operator * (f32 rhs);
	Matrix4x4_f32 operator / (f32 rhs);
};

Matrix4x4_f32  matrix4x4_f32_transpose(Matrix4x4_f32 matrix);
          f32  matrix4x4_f32_determinant(Matrix4x4_f32 matrix);
Matrix4x4_f32  matrix4x4_f32_inverse(Matrix4x4_f32 matrix);
Matrix4x4_f32  matrix4x4_f32_multiply(Matrix4x4_f32 lhs, Matrix4x4_f32 rhs);

struct Matrix4x4_f64 {
	Vector4_f64 columns[4];

	Matrix4x4_f64();
	Matrix4x4_f64(f64 value);
	Matrix4x4_f64(Vector4_f64 column0, Vector4_f64 column1, Vector4_f64 column2, Vector4_f64 column3);

	bool operator == (Matrix4x4_f64 rhs);
	bool operator == (f64 rhs);

	Vector4_f64 & operator [] (int index);

	Matrix4x4_f64 & operator += (Matrix4x4_f64 rhs);
	Matrix4x4_f64 & operator -= (Matrix4x4_f64 rhs);
	Matrix4x4_f64 & operator *= (Matrix4x4_f64 rhs);
	Matrix4x4_f64 & operator /= (Matrix4x4_f64 rhs);

	Matrix4x4_f64 & operator += (f64 rhs);
	Matrix4x4_f64 & operator -= (f64 rhs);
	Matrix4x4_f64 & operator *= (f64 rhs);
	Matrix4x4_f64 & operator /= (f64 rhs);

	Matrix4x4_f64 operator + (Matrix4x4_f64 rhs);
	Matrix4x4_f64 operator - (Matrix4x4_f64 rhs);
	Matrix4x4_f64 operator * (Matrix4x4_f64 rhs);
	Matrix4x4_f64 operator / (Matrix4x4_f64 rhs);

	Matrix4x4_f64 operator + (f64 rhs);
	Matrix4x4_f64 operator - (f64 rhs);
	Matrix4x4_f64 operator * (f64 rhs);
	Matrix4x4_f64 operator / (f64 rhs);
};

Matrix4x4_f64  matrix4x4_f64_transpose(Matrix4x4_f32 matrix);
          f64  matrix4x4_f64_determinant(Matrix4x4_f32 matrix);
Matrix4x4_f64  matrix4x4_f64_inverse(Matrix4x4_f32 matrix);
Matrix4x4_f64  matrix4x4_f64_multiply(Matrix4x4_f32 lhs, Matrix4x4_f32 rhs);

constexpr Vector3_f32 WORLD_DIRECTION_RIGHT   = Vector3_f32( 1.0f,  0.0f,  0.0f );
constexpr Vector3_f32 WORLD_DIRECTION_UP      = Vector3_f32( 0.0f,  1.0f,  0.0f );
constexpr Vector3_f32 WORLD_DIRECTION_FORWARD = Vector3_f32( 0.0f,  0.0f,  1.0f );

#endif /* QLIGHT_MATH_H */