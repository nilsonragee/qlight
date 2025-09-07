#include <assert.h>
#include <xmmintrin.h>
#include <math.h>

#include "math.h"

f32 radians( f32 degrees ) {
	f32 radians = degrees * ( PI / 180.0f );
	return radians;
}

f32 degrees( f32 radians ) {
	f32 degrees = radians * ( 180.0f / PI );
	return degrees;
}

f32 inverse_sqrt_fast( f32 x ) {
	__m128 xmm_register = _mm_set_ss( x );
	xmm_register = _mm_rsqrt_ss( xmm_register );
	return _mm_cvtss_f32( xmm_register );
}

f32 inverse_sqrt_refined( f32 x ) {
	__m128 xmm = _mm_set_ss( x );
	xmm = _mm_rsqrt_ss( xmm );
	f32 y = _mm_cvtss_f32( xmm );

	// One Newton's method iteration for better approximation.
	y = y * ( 1.5f  -  0.5f * x * y * y );
	return y;
}

f32 inverse_sqrt_general( f32 x ) {
	return 1.0f / sqrtf( x );
}

Vector2_f32 cross( Vector2_f32 lhs, Vector2_f32 rhs ) {
	Vector2_f32 result = {
		lhs.y * rhs.x  -  rhs.y * lhs.x,
		lhs.x * rhs.y  -  rhs.x * lhs.y
	};

	return result;
}

Vector3_f32 cross( Vector3_f32 lhs, Vector3_f32 rhs ) {
	Vector3_f32 result = {
		lhs.y * rhs.z  -  rhs.y * lhs.z,
		lhs.z * rhs.x  -  rhs.z * lhs.x,
		lhs.x * rhs.y  -  rhs.x * lhs.y
	};

	return result;
}

f32 dot( Vector2_f32 lhs, Vector2_f32 rhs ) {
	Vector2_f32 vector = lhs * rhs;
	f32 result = vector.x + vector.y;
	return result;
}

f32 dot( Vector3_f32 lhs, Vector3_f32 rhs ) {
	Vector3_f32 vector = lhs * rhs;
	f32 result = vector.x + vector.y + vector.z;
	return result;
}

f32 dot( Vector4_f32 lhs, Vector4_f32 rhs ) {
	Vector4_f32 vector = lhs * rhs;
	f32 result = vector.x + vector.y + vector.z + vector.w;
	return result;
}

Vector3_f32 normalize( Vector3_f32 v ) {
	// `inverse_sqrt_fast` produces significant inaccuracy.
	// `inverse_sqrt_refined` yields a very close approximation
	//  to a precise result, but still is not exactly correct.
	//
	// For example:
	// v = ( 0, 1, 0 );
	// inverse_sqrt_fast( dot( v, v ) ) = 0.999755859;
	// inverse_sqrt_refined( dot( v, v ) ) = 0.999999940;
	// inverse_sqrt_general( dot( v, v ) ) = 1;
	//
	// Normalizing is probably the last place expected to
	//  include inaccuracies, even marginal, so let us
	//  keep it precise.

	// Vector3_f32 fast = v * inverse_sqrt_fast( dot( v, v ) );
	// Vector3_f32 refined = v * inverse_sqrt_refined( dot( v, v ) );
	Vector3_f32 result = v * inverse_sqrt_general( dot( v, v ) );
	return result;
}

// Sections:
// [SECTION] Operator overloadings: Vector2_s8
// [SECTION] Operator overloadings: Vector2_u8
// [SECTION] Operator overloadings: Vector2_s16
// [SECTION] Operator overloadings: Vector2_u16
// [SECTION] Operator overloadings: Vector2_s32
// [SECTION] Operator overloadings: Vector2_u32
// [SECTION] Operator overloadings: Vector2_s64
// [SECTION] Operator overloadings: Vector2_u64
// [SECTION] Operator overloadings: Vector2_f32
// [SECTION] Operator overloadings: Vector2_f64
// [SECTION] Operator overloadings: Vector3_s8
// [SECTION] Operator overloadings: Vector3_u8
// [SECTION] Operator overloadings: Vector3_s16
// [SECTION] Operator overloadings: Vector3_u16
// [SECTION] Operator overloadings: Vector3_s32
// [SECTION] Operator overloadings: Vector3_u32
// [SECTION] Operator overloadings: Vector3_s64
// [SECTION] Operator overloadings: Vector3_u64
// [SECTION] Operator overloadings: Vector3_f32
// [SECTION] Operator overloadings: Vector3_f64
// [SECTION] Operator overloadings: Vector4_s8
// [SECTION] Operator overloadings: Vector4_u8
// [SECTION] Operator overloadings: Vector4_s16
// [SECTION] Operator overloadings: Vector4_u16
// [SECTION] Operator overloadings: Vector4_s32
// [SECTION] Operator overloadings: Vector4_u32
// [SECTION] Operator overloadings: Vector4_s64
// [SECTION] Operator overloadings: Vector4_u64
// [SECTION] Operator overloadings: Vector4_f32
// [SECTION] Operator overloadings: Vector4_f64
// [SECTION] Matrix3x3_f32 matrix functions
// [SECTION] Operator overloadings: Matrix3x3_f32
// [SECTION] Matrix3x3_f64 matrix functions
// [SECTION] Operator overloadings: Matrix3x3_f64
// [SECTION] Matrix4x4_f32 matrix functions
// [SECTION] Operator overloadings: Matrix4x4_f32
// [SECTION] Matrix4x4_f64 matrix functions
// [SECTION] Operator overloadings: Matrix4x4_f64

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector2_s8
//-----------------------------------------------------------------------------

bool Vector2_s8::operator == (Vector2_s8 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y;
}

bool Vector2_s8::operator == (s8 rhs) {
	return this->x == rhs &&
	       this->y == rhs;
}

s8 & Vector2_s8::operator [] (int index) {
	assert(index >= 0 && index < 2);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
	}
}

Vector2_s8 & Vector2_s8::operator += (Vector2_s8 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	return *this;
}

Vector2_s8 & Vector2_s8::operator -= (Vector2_s8 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	return *this;
}

Vector2_s8 & Vector2_s8::operator *= (Vector2_s8 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	return *this;
}

Vector2_s8 & Vector2_s8::operator /= (Vector2_s8 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	return *this;
}

Vector2_s8 & Vector2_s8::operator %= (Vector2_s8 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	return *this;
}

Vector2_s8 & Vector2_s8::operator += (s8 rhs) {
	this->x += rhs;
	this->y += rhs;
	return *this;
}

Vector2_s8 & Vector2_s8::operator -= (s8 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	return *this;
}

Vector2_s8 & Vector2_s8::operator *= (s8 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	return *this;
}

Vector2_s8 & Vector2_s8::operator /= (s8 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	return *this;
}

Vector2_s8 & Vector2_s8::operator %= (s8 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	return *this;
}

Vector2_s8 Vector2_s8::operator + (Vector2_s8 rhs) {
	return Vector2_s8 {
		static_cast<s8>(this->x + rhs.x),
		static_cast<s8>(this->y + rhs.y)
	};
}

Vector2_s8 Vector2_s8::operator - (Vector2_s8 rhs) {
	return Vector2_s8 {
		static_cast<s8>(this->x - rhs.x),
		static_cast<s8>(this->y - rhs.y)
	};
}

Vector2_s8 Vector2_s8::operator * (Vector2_s8 rhs) {
	return Vector2_s8 {
		static_cast<s8>(this->x * rhs.x),
		static_cast<s8>(this->y * rhs.y)
	};
}

Vector2_s8 Vector2_s8::operator / (Vector2_s8 rhs) {
	return Vector2_s8 {
		static_cast<s8>(this->x / rhs.x),
		static_cast<s8>(this->y / rhs.y)
	};
}

Vector2_s8 Vector2_s8::operator % (Vector2_s8 rhs) {
	return Vector2_s8 {
		static_cast<s8>(this->x % rhs.x),
		static_cast<s8>(this->y % rhs.y)
	};
}

Vector2_s8 Vector2_s8::operator + (s8 rhs)  {
	return Vector2_s8 {
		static_cast<s8>(this->x + rhs),
		static_cast<s8>(this->y + rhs)
	};
}

Vector2_s8 Vector2_s8::operator - (s8 rhs) {
	return Vector2_s8 {
		static_cast<s8>(this->x - rhs),
		static_cast<s8>(this->y - rhs)
	};
}

Vector2_s8 Vector2_s8::operator * (s8 rhs) {
	return Vector2_s8 {
		static_cast<s8>(this->x * rhs),
		static_cast<s8>(this->y * rhs)
	};
}

Vector2_s8 Vector2_s8::operator / (s8 rhs) {
	return Vector2_s8 {
		static_cast<s8>(this->x / rhs),
		static_cast<s8>(this->y / rhs)
	};
}

Vector2_s8 Vector2_s8::operator % (s8 rhs) {
	return Vector2_s8 {
		static_cast<s8>(this->x % rhs),
		static_cast<s8>(this->y % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector2_u8
//-----------------------------------------------------------------------------

bool Vector2_u8::operator == (Vector2_u8 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y;
}

bool Vector2_u8::operator == (u8 rhs) {
	return this->x == rhs &&
	       this->y == rhs;
}

u8 & Vector2_u8::operator [] (int index) {
	assert(index >= 0 && index < 2);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
	}
}

Vector2_u8 & Vector2_u8::operator += (Vector2_u8 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	return *this;
}

Vector2_u8 & Vector2_u8::operator -= (Vector2_u8 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	return *this;
}

Vector2_u8 & Vector2_u8::operator *= (Vector2_u8 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	return *this;
}

Vector2_u8 & Vector2_u8::operator /= (Vector2_u8 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	return *this;
}

Vector2_u8 & Vector2_u8::operator %= (Vector2_u8 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	return *this;
}

Vector2_u8 & Vector2_u8::operator += (u8 rhs) {
	this->x += rhs;
	this->y += rhs;
	return *this;
}

Vector2_u8 & Vector2_u8::operator -= (u8 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	return *this;
}

Vector2_u8 & Vector2_u8::operator *= (u8 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	return *this;
}

Vector2_u8 & Vector2_u8::operator /= (u8 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	return *this;
}

Vector2_u8 & Vector2_u8::operator %= (u8 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	return *this;
}

Vector2_u8 Vector2_u8::operator + (Vector2_u8 rhs) {
	return Vector2_u8 {
		static_cast<u8>(this->x + rhs.x),
		static_cast<u8>(this->y + rhs.y)
	};
}

Vector2_u8 Vector2_u8::operator - (Vector2_u8 rhs) {
	return Vector2_u8 {
		static_cast<u8>(this->x - rhs.x),
		static_cast<u8>(this->y - rhs.y)
	};
}

Vector2_u8 Vector2_u8::operator * (Vector2_u8 rhs) {
	return Vector2_u8 {
		static_cast<u8>(this->x * rhs.x),
		static_cast<u8>(this->y * rhs.y)
	};
}

Vector2_u8 Vector2_u8::operator / (Vector2_u8 rhs) {
	return Vector2_u8 {
		static_cast<u8>(this->x / rhs.x),
		static_cast<u8>(this->y / rhs.y)
	};
}

Vector2_u8 Vector2_u8::operator % (Vector2_u8 rhs) {
	return Vector2_u8 {
		static_cast<u8>(this->x % rhs.x),
		static_cast<u8>(this->y % rhs.y)
	};
}

Vector2_u8 Vector2_u8::operator + (u8 rhs)  {
	return Vector2_u8 {
		static_cast<u8>(this->x + rhs),
		static_cast<u8>(this->y + rhs)
	};
}

Vector2_u8 Vector2_u8::operator - (u8 rhs) {
	return Vector2_u8 {
		static_cast<u8>(this->x - rhs),
		static_cast<u8>(this->y - rhs)
	};
}

Vector2_u8 Vector2_u8::operator * (u8 rhs) {
	return Vector2_u8 {
		static_cast<u8>(this->x * rhs),
		static_cast<u8>(this->y * rhs)
	};
}

Vector2_u8 Vector2_u8::operator / (u8 rhs) {
	return Vector2_u8 {
		static_cast<u8>(this->x / rhs),
		static_cast<u8>(this->y / rhs)
	};
}

Vector2_u8 Vector2_u8::operator % (u8 rhs) {
	return Vector2_u8 {
		static_cast<u8>(this->x % rhs),
		static_cast<u8>(this->y % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector2_s16
//-----------------------------------------------------------------------------

bool Vector2_s16::operator == (Vector2_s16 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y;
}

bool Vector2_s16::operator == (s16 rhs) {
	return this->x == rhs &&
	       this->y == rhs;
}

s16 & Vector2_s16::operator [] (int index) {
	assert(index >= 0 && index < 2);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
	}
}

Vector2_s16 & Vector2_s16::operator += (Vector2_s16 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	return *this;
}

Vector2_s16 & Vector2_s16::operator -= (Vector2_s16 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	return *this;
}

Vector2_s16 & Vector2_s16::operator *= (Vector2_s16 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	return *this;
}

Vector2_s16 & Vector2_s16::operator /= (Vector2_s16 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	return *this;
}

Vector2_s16 & Vector2_s16::operator %= (Vector2_s16 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	return *this;
}

Vector2_s16 & Vector2_s16::operator += (s16 rhs) {
	this->x += rhs;
	this->y += rhs;
	return *this;
}

Vector2_s16 & Vector2_s16::operator -= (s16 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	return *this;
}

Vector2_s16 & Vector2_s16::operator *= (s16 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	return *this;
}

Vector2_s16 & Vector2_s16::operator /= (s16 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	return *this;
}

Vector2_s16 & Vector2_s16::operator %= (s16 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	return *this;
}

Vector2_s16 Vector2_s16::operator + (Vector2_s16 rhs) {
	return Vector2_s16 {
		static_cast<s16>(this->x + rhs.x),
		static_cast<s16>(this->y + rhs.y)
	};
}

Vector2_s16 Vector2_s16::operator - (Vector2_s16 rhs) {
	return Vector2_s16 {
		static_cast<s16>(this->x - rhs.x),
		static_cast<s16>(this->y - rhs.y)
	};
}

Vector2_s16 Vector2_s16::operator * (Vector2_s16 rhs) {
	return Vector2_s16 {
		static_cast<s16>(this->x * rhs.x),
		static_cast<s16>(this->y * rhs.y)
	};
}

Vector2_s16 Vector2_s16::operator / (Vector2_s16 rhs) {
	return Vector2_s16 {
		static_cast<s16>(this->x / rhs.x),
		static_cast<s16>(this->y / rhs.y)
	};
}

Vector2_s16 Vector2_s16::operator % (Vector2_s16 rhs) {
	return Vector2_s16 {
		static_cast<s16>(this->x % rhs.x),
		static_cast<s16>(this->y % rhs.y)
	};
}

Vector2_s16 Vector2_s16::operator + (s16 rhs)  {
	return Vector2_s16 {
		static_cast<s16>(this->x + rhs),
		static_cast<s16>(this->y + rhs)
	};
}

Vector2_s16 Vector2_s16::operator - (s16 rhs) {
	return Vector2_s16 {
		static_cast<s16>(this->x - rhs),
		static_cast<s16>(this->y - rhs)
	};
}

Vector2_s16 Vector2_s16::operator * (s16 rhs) {
	return Vector2_s16 {
		static_cast<s16>(this->x * rhs),
		static_cast<s16>(this->y * rhs)
	};
}

Vector2_s16 Vector2_s16::operator / (s16 rhs) {
	return Vector2_s16 {
		static_cast<s16>(this->x / rhs),
		static_cast<s16>(this->y / rhs)
	};
}

Vector2_s16 Vector2_s16::operator % (s16 rhs) {
	return Vector2_s16 {
		static_cast<s16>(this->x % rhs),
		static_cast<s16>(this->y % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector2_u16
//-----------------------------------------------------------------------------

bool Vector2_u16::operator == (Vector2_u16 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y;
}

bool Vector2_u16::operator == (u16 rhs) {
	return this->x == rhs &&
	       this->y == rhs;
}

u16 & Vector2_u16::operator [] (int index) {
	assert(index >= 0 && index < 2);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
	}
}

Vector2_u16 & Vector2_u16::operator += (Vector2_u16 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	return *this;
}

Vector2_u16 & Vector2_u16::operator -= (Vector2_u16 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	return *this;
}

Vector2_u16 & Vector2_u16::operator *= (Vector2_u16 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	return *this;
}

Vector2_u16 & Vector2_u16::operator /= (Vector2_u16 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	return *this;
}

Vector2_u16 & Vector2_u16::operator %= (Vector2_u16 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	return *this;
}

Vector2_u16 & Vector2_u16::operator += (u16 rhs) {
	this->x += rhs;
	this->y += rhs;
	return *this;
}

Vector2_u16 & Vector2_u16::operator -= (u16 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	return *this;
}

Vector2_u16 & Vector2_u16::operator *= (u16 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	return *this;
}

Vector2_u16 & Vector2_u16::operator /= (u16 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	return *this;
}

Vector2_u16 & Vector2_u16::operator %= (u16 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	return *this;
}

Vector2_u16 Vector2_u16::operator + (Vector2_u16 rhs) {
	return Vector2_u16 {
		static_cast<u16>(this->x + rhs.x),
		static_cast<u16>(this->y + rhs.y)
	};
}

Vector2_u16 Vector2_u16::operator - (Vector2_u16 rhs) {
	return Vector2_u16 {
		static_cast<u16>(this->x - rhs.x),
		static_cast<u16>(this->y - rhs.y)
	};
}

Vector2_u16 Vector2_u16::operator * (Vector2_u16 rhs) {
	return Vector2_u16 {
		static_cast<u16>(this->x * rhs.x),
		static_cast<u16>(this->y * rhs.y)
	};
}

Vector2_u16 Vector2_u16::operator / (Vector2_u16 rhs) {
	return Vector2_u16 {
		static_cast<u16>(this->x / rhs.x),
		static_cast<u16>(this->y / rhs.y)
	};
}

Vector2_u16 Vector2_u16::operator % (Vector2_u16 rhs) {
	return Vector2_u16 {
		static_cast<u16>(this->x % rhs.x),
		static_cast<u16>(this->y % rhs.y)
	};
}

Vector2_u16 Vector2_u16::operator + (u16 rhs)  {
	return Vector2_u16 {
		static_cast<u16>(this->x + rhs),
		static_cast<u16>(this->y + rhs)
	};
}

Vector2_u16 Vector2_u16::operator - (u16 rhs) {
	return Vector2_u16 {
		static_cast<u16>(this->x - rhs),
		static_cast<u16>(this->y - rhs)
	};
}

Vector2_u16 Vector2_u16::operator * (u16 rhs) {
	return Vector2_u16 {
		static_cast<u16>(this->x * rhs),
		static_cast<u16>(this->y * rhs)
	};
}

Vector2_u16 Vector2_u16::operator / (u16 rhs) {
	return Vector2_u16 {
		static_cast<u16>(this->x / rhs),
		static_cast<u16>(this->y / rhs)
	};
}

Vector2_u16 Vector2_u16::operator % (u16 rhs) {
	return Vector2_u16 {
		static_cast<u16>(this->x % rhs),
		static_cast<u16>(this->y % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector2_s32
//-----------------------------------------------------------------------------

bool Vector2_s32::operator == (Vector2_s32 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y;
}

bool Vector2_s32::operator == (s32 rhs) {
	return this->x == rhs &&
	       this->y == rhs;
}

s32 & Vector2_s32::operator [] (int index) {
	assert(index >= 0 && index < 2);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
	}
}

Vector2_s32 & Vector2_s32::operator += (Vector2_s32 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	return *this;
}

Vector2_s32 & Vector2_s32::operator -= (Vector2_s32 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	return *this;
}

Vector2_s32 & Vector2_s32::operator *= (Vector2_s32 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	return *this;
}

Vector2_s32 & Vector2_s32::operator /= (Vector2_s32 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	return *this;
}

Vector2_s32 & Vector2_s32::operator %= (Vector2_s32 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	return *this;
}

Vector2_s32 & Vector2_s32::operator += (s32 rhs) {
	this->x += rhs;
	this->y += rhs;
	return *this;
}

Vector2_s32 & Vector2_s32::operator -= (s32 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	return *this;
}

Vector2_s32 & Vector2_s32::operator *= (s32 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	return *this;
}

Vector2_s32 & Vector2_s32::operator /= (s32 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	return *this;
}

Vector2_s32 & Vector2_s32::operator %= (s32 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	return *this;
}

Vector2_s32 Vector2_s32::operator + (Vector2_s32 rhs) {
	return Vector2_s32 {
		static_cast<s32>(this->x + rhs.x),
		static_cast<s32>(this->y + rhs.y)
	};
}

Vector2_s32 Vector2_s32::operator - (Vector2_s32 rhs) {
	return Vector2_s32 {
		static_cast<s32>(this->x - rhs.x),
		static_cast<s32>(this->y - rhs.y)
	};
}

Vector2_s32 Vector2_s32::operator * (Vector2_s32 rhs) {
	return Vector2_s32 {
		static_cast<s32>(this->x * rhs.x),
		static_cast<s32>(this->y * rhs.y)
	};
}

Vector2_s32 Vector2_s32::operator / (Vector2_s32 rhs) {
	return Vector2_s32 {
		static_cast<s32>(this->x / rhs.x),
		static_cast<s32>(this->y / rhs.y)
	};
}

Vector2_s32 Vector2_s32::operator % (Vector2_s32 rhs) {
	return Vector2_s32 {
		static_cast<s32>(this->x % rhs.x),
		static_cast<s32>(this->y % rhs.y)
	};
}

Vector2_s32 Vector2_s32::operator + (s32 rhs)  {
	return Vector2_s32 {
		static_cast<s32>(this->x + rhs),
		static_cast<s32>(this->y + rhs)
	};
}

Vector2_s32 Vector2_s32::operator - (s32 rhs) {
	return Vector2_s32 {
		static_cast<s32>(this->x - rhs),
		static_cast<s32>(this->y - rhs)
	};
}

Vector2_s32 Vector2_s32::operator * (s32 rhs) {
	return Vector2_s32 {
		static_cast<s32>(this->x * rhs),
		static_cast<s32>(this->y * rhs)
	};
}

Vector2_s32 Vector2_s32::operator / (s32 rhs) {
	return Vector2_s32 {
		static_cast<s32>(this->x / rhs),
		static_cast<s32>(this->y / rhs)
	};
}

Vector2_s32 Vector2_s32::operator % (s32 rhs) {
	return Vector2_s32 {
		static_cast<s32>(this->x % rhs),
		static_cast<s32>(this->y % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector2_u32
//-----------------------------------------------------------------------------

bool Vector2_u32::operator == (Vector2_u32 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y;
}

bool Vector2_u32::operator == (u32 rhs) {
	return this->x == rhs &&
	       this->y == rhs;
}

u32 & Vector2_u32::operator [] (int index) {
	assert(index >= 0 && index < 2);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
	}
}

Vector2_u32 & Vector2_u32::operator += (Vector2_u32 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	return *this;
}

Vector2_u32 & Vector2_u32::operator -= (Vector2_u32 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	return *this;
}

Vector2_u32 & Vector2_u32::operator *= (Vector2_u32 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	return *this;
}

Vector2_u32 & Vector2_u32::operator /= (Vector2_u32 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	return *this;
}

Vector2_u32 & Vector2_u32::operator %= (Vector2_u32 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	return *this;
}

Vector2_u32 & Vector2_u32::operator += (u32 rhs) {
	this->x += rhs;
	this->y += rhs;
	return *this;
}

Vector2_u32 & Vector2_u32::operator -= (u32 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	return *this;
}

Vector2_u32 & Vector2_u32::operator *= (u32 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	return *this;
}

Vector2_u32 & Vector2_u32::operator /= (u32 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	return *this;
}

Vector2_u32 & Vector2_u32::operator %= (u32 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	return *this;
}

Vector2_u32 Vector2_u32::operator + (Vector2_u32 rhs) {
	return Vector2_u32 {
		static_cast<u32>(this->x + rhs.x),
		static_cast<u32>(this->y + rhs.y)
	};
}

Vector2_u32 Vector2_u32::operator - (Vector2_u32 rhs) {
	return Vector2_u32 {
		static_cast<u32>(this->x - rhs.x),
		static_cast<u32>(this->y - rhs.y)
	};
}

Vector2_u32 Vector2_u32::operator * (Vector2_u32 rhs) {
	return Vector2_u32 {
		static_cast<u32>(this->x * rhs.x),
		static_cast<u32>(this->y * rhs.y)
	};
}

Vector2_u32 Vector2_u32::operator / (Vector2_u32 rhs) {
	return Vector2_u32 {
		static_cast<u32>(this->x / rhs.x),
		static_cast<u32>(this->y / rhs.y)
	};
}

Vector2_u32 Vector2_u32::operator % (Vector2_u32 rhs) {
	return Vector2_u32 {
		static_cast<u32>(this->x % rhs.x),
		static_cast<u32>(this->y % rhs.y)
	};
}

Vector2_u32 Vector2_u32::operator + (u32 rhs)  {
	return Vector2_u32 {
		static_cast<u32>(this->x + rhs),
		static_cast<u32>(this->y + rhs)
	};
}

Vector2_u32 Vector2_u32::operator - (u32 rhs) {
	return Vector2_u32 {
		static_cast<u32>(this->x - rhs),
		static_cast<u32>(this->y - rhs)
	};
}

Vector2_u32 Vector2_u32::operator * (u32 rhs) {
	return Vector2_u32 {
		static_cast<u32>(this->x * rhs),
		static_cast<u32>(this->y * rhs)
	};
}

Vector2_u32 Vector2_u32::operator / (u32 rhs) {
	return Vector2_u32 {
		static_cast<u32>(this->x / rhs),
		static_cast<u32>(this->y / rhs)
	};
}

Vector2_u32 Vector2_u32::operator % (u32 rhs) {
	return Vector2_u32 {
		static_cast<u32>(this->x % rhs),
		static_cast<u32>(this->y % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector2_s64
//-----------------------------------------------------------------------------

bool Vector2_s64::operator == (Vector2_s64 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y;
}

bool Vector2_s64::operator == (s64 rhs) {
	return this->x == rhs &&
	       this->y == rhs;
}

s64 & Vector2_s64::operator [] (int index) {
	assert(index >= 0 && index < 2);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
	}
}

Vector2_s64 & Vector2_s64::operator += (Vector2_s64 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	return *this;
}

Vector2_s64 & Vector2_s64::operator -= (Vector2_s64 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	return *this;
}

Vector2_s64 & Vector2_s64::operator *= (Vector2_s64 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	return *this;
}

Vector2_s64 & Vector2_s64::operator /= (Vector2_s64 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	return *this;
}

Vector2_s64 & Vector2_s64::operator %= (Vector2_s64 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	return *this;
}

Vector2_s64 & Vector2_s64::operator += (s64 rhs) {
	this->x += rhs;
	this->y += rhs;
	return *this;
}

Vector2_s64 & Vector2_s64::operator -= (s64 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	return *this;
}

Vector2_s64 & Vector2_s64::operator *= (s64 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	return *this;
}

Vector2_s64 & Vector2_s64::operator /= (s64 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	return *this;
}

Vector2_s64 & Vector2_s64::operator %= (s64 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	return *this;
}

Vector2_s64 Vector2_s64::operator + (Vector2_s64 rhs) {
	return Vector2_s64 {
		static_cast<s64>(this->x + rhs.x),
		static_cast<s64>(this->y + rhs.y)
	};
}

Vector2_s64 Vector2_s64::operator - (Vector2_s64 rhs) {
	return Vector2_s64 {
		static_cast<s64>(this->x - rhs.x),
		static_cast<s64>(this->y - rhs.y)
	};
}

Vector2_s64 Vector2_s64::operator * (Vector2_s64 rhs) {
	return Vector2_s64 {
		static_cast<s64>(this->x * rhs.x),
		static_cast<s64>(this->y * rhs.y)
	};
}

Vector2_s64 Vector2_s64::operator / (Vector2_s64 rhs) {
	return Vector2_s64 {
		static_cast<s64>(this->x / rhs.x),
		static_cast<s64>(this->y / rhs.y)
	};
}

Vector2_s64 Vector2_s64::operator % (Vector2_s64 rhs) {
	return Vector2_s64 {
		static_cast<s64>(this->x % rhs.x),
		static_cast<s64>(this->y % rhs.y)
	};
}

Vector2_s64 Vector2_s64::operator + (s64 rhs)  {
	return Vector2_s64 {
		static_cast<s64>(this->x + rhs),
		static_cast<s64>(this->y + rhs)
	};
}

Vector2_s64 Vector2_s64::operator - (s64 rhs) {
	return Vector2_s64 {
		static_cast<s64>(this->x - rhs),
		static_cast<s64>(this->y - rhs)
	};
}

Vector2_s64 Vector2_s64::operator * (s64 rhs) {
	return Vector2_s64 {
		static_cast<s64>(this->x * rhs),
		static_cast<s64>(this->y * rhs)
	};
}

Vector2_s64 Vector2_s64::operator / (s64 rhs) {
	return Vector2_s64 {
		static_cast<s64>(this->x / rhs),
		static_cast<s64>(this->y / rhs)
	};
}

Vector2_s64 Vector2_s64::operator % (s64 rhs) {
	return Vector2_s64 {
		static_cast<s64>(this->x % rhs),
		static_cast<s64>(this->y % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector2_u64
//-----------------------------------------------------------------------------

bool Vector2_u64::operator == (Vector2_u64 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y;
}

bool Vector2_u64::operator == (u64 rhs) {
	return this->x == rhs &&
	       this->y == rhs;
}

u64 & Vector2_u64::operator [] (int index) {
	assert(index >= 0 && index < 2);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
	}
}

Vector2_u64 & Vector2_u64::operator += (Vector2_u64 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	return *this;
}

Vector2_u64 & Vector2_u64::operator -= (Vector2_u64 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	return *this;
}

Vector2_u64 & Vector2_u64::operator *= (Vector2_u64 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	return *this;
}

Vector2_u64 & Vector2_u64::operator /= (Vector2_u64 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	return *this;
}

Vector2_u64 & Vector2_u64::operator %= (Vector2_u64 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	return *this;
}

Vector2_u64 & Vector2_u64::operator += (u64 rhs) {
	this->x += rhs;
	this->y += rhs;
	return *this;
}

Vector2_u64 & Vector2_u64::operator -= (u64 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	return *this;
}

Vector2_u64 & Vector2_u64::operator *= (u64 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	return *this;
}

Vector2_u64 & Vector2_u64::operator /= (u64 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	return *this;
}

Vector2_u64 & Vector2_u64::operator %= (u64 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	return *this;
}

Vector2_u64 Vector2_u64::operator + (Vector2_u64 rhs) {
	return Vector2_u64 {
		static_cast<u64>(this->x + rhs.x),
		static_cast<u64>(this->y + rhs.y)
	};
}

Vector2_u64 Vector2_u64::operator - (Vector2_u64 rhs) {
	return Vector2_u64 {
		static_cast<u64>(this->x - rhs.x),
		static_cast<u64>(this->y - rhs.y)
	};
}

Vector2_u64 Vector2_u64::operator * (Vector2_u64 rhs) {
	return Vector2_u64 {
		static_cast<u64>(this->x * rhs.x),
		static_cast<u64>(this->y * rhs.y)
	};
}

Vector2_u64 Vector2_u64::operator / (Vector2_u64 rhs) {
	return Vector2_u64 {
		static_cast<u64>(this->x / rhs.x),
		static_cast<u64>(this->y / rhs.y)
	};
}

Vector2_u64 Vector2_u64::operator % (Vector2_u64 rhs) {
	return Vector2_u64 {
		static_cast<u64>(this->x % rhs.x),
		static_cast<u64>(this->y % rhs.y)
	};
}

Vector2_u64 Vector2_u64::operator + (u64 rhs)  {
	return Vector2_u64 {
		static_cast<u64>(this->x + rhs),
		static_cast<u64>(this->y + rhs)
	};
}

Vector2_u64 Vector2_u64::operator - (u64 rhs) {
	return Vector2_u64 {
		static_cast<u64>(this->x - rhs),
		static_cast<u64>(this->y - rhs)
	};
}

Vector2_u64 Vector2_u64::operator * (u64 rhs) {
	return Vector2_u64 {
		static_cast<u64>(this->x * rhs),
		static_cast<u64>(this->y * rhs)
	};
}

Vector2_u64 Vector2_u64::operator / (u64 rhs) {
	return Vector2_u64 {
		static_cast<u64>(this->x / rhs),
		static_cast<u64>(this->y / rhs)
	};
}

Vector2_u64 Vector2_u64::operator % (u64 rhs) {
	return Vector2_u64 {
		static_cast<u64>(this->x % rhs),
		static_cast<u64>(this->y % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector2_f32
//-----------------------------------------------------------------------------

bool Vector2_f32::operator == (Vector2_f32 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y;
}

bool Vector2_f32::operator == (f32 rhs) {
	return this->x == rhs &&
	       this->y == rhs;
}

f32 & Vector2_f32::operator [] (int index) {
	assert(index >= 0 && index < 2);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
	}
}

Vector2_f32 & Vector2_f32::operator += (Vector2_f32 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	return *this;
}

Vector2_f32 & Vector2_f32::operator -= (Vector2_f32 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	return *this;
}

Vector2_f32 & Vector2_f32::operator *= (Vector2_f32 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	return *this;
}

Vector2_f32 & Vector2_f32::operator /= (Vector2_f32 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	return *this;
}

Vector2_f32 & Vector2_f32::operator += (f32 rhs) {
	this->x += rhs;
	this->y += rhs;
	return *this;
}

Vector2_f32 & Vector2_f32::operator -= (f32 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	return *this;
}

Vector2_f32 & Vector2_f32::operator *= (f32 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	return *this;
}

Vector2_f32 & Vector2_f32::operator /= (f32 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	return *this;
}

Vector2_f32 Vector2_f32::operator + (Vector2_f32 rhs) {
	return Vector2_f32 {
		static_cast<f32>(this->x + rhs.x),
		static_cast<f32>(this->y + rhs.y)
	};
}

Vector2_f32 Vector2_f32::operator - (Vector2_f32 rhs) {
	return Vector2_f32 {
		static_cast<f32>(this->x - rhs.x),
		static_cast<f32>(this->y - rhs.y)
	};
}

Vector2_f32 Vector2_f32::operator * (Vector2_f32 rhs) {
	return Vector2_f32 {
		static_cast<f32>(this->x * rhs.x),
		static_cast<f32>(this->y * rhs.y)
	};
}

Vector2_f32 Vector2_f32::operator / (Vector2_f32 rhs) {
	return Vector2_f32 {
		static_cast<f32>(this->x / rhs.x),
		static_cast<f32>(this->y / rhs.y)
	};
}

Vector2_f32 Vector2_f32::operator + (f32 rhs)  {
	return Vector2_f32 {
		static_cast<f32>(this->x + rhs),
		static_cast<f32>(this->y + rhs)
	};
}

Vector2_f32 Vector2_f32::operator - (f32 rhs) {
	return Vector2_f32 {
		static_cast<f32>(this->x - rhs),
		static_cast<f32>(this->y - rhs)
	};
}

Vector2_f32 Vector2_f32::operator * (f32 rhs) {
	return Vector2_f32 {
		static_cast<f32>(this->x * rhs),
		static_cast<f32>(this->y * rhs)
	};
}

Vector2_f32 Vector2_f32::operator / (f32 rhs) {
	return Vector2_f32 {
		static_cast<f32>(this->x / rhs),
		static_cast<f32>(this->y / rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector2_f64
//-----------------------------------------------------------------------------

bool Vector2_f64::operator == (Vector2_f64 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y;
}

bool Vector2_f64::operator == (f64 rhs) {
	return this->x == rhs &&
	       this->y == rhs;
}

f64 & Vector2_f64::operator [] (int index) {
	assert(index >= 0 && index < 2);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
	}
}

Vector2_f64 & Vector2_f64::operator += (Vector2_f64 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	return *this;
}

Vector2_f64 & Vector2_f64::operator -= (Vector2_f64 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	return *this;
}

Vector2_f64 & Vector2_f64::operator *= (Vector2_f64 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	return *this;
}

Vector2_f64 & Vector2_f64::operator /= (Vector2_f64 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	return *this;
}

Vector2_f64 & Vector2_f64::operator += (f64 rhs) {
	this->x += rhs;
	this->y += rhs;
	return *this;
}

Vector2_f64 & Vector2_f64::operator -= (f64 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	return *this;
}

Vector2_f64 & Vector2_f64::operator *= (f64 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	return *this;
}

Vector2_f64 & Vector2_f64::operator /= (f64 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	return *this;
}

Vector2_f64 Vector2_f64::operator + (Vector2_f64 rhs) {
	return Vector2_f64 {
		static_cast<f64>(this->x + rhs.x),
		static_cast<f64>(this->y + rhs.y)
	};
}

Vector2_f64 Vector2_f64::operator - (Vector2_f64 rhs) {
	return Vector2_f64 {
		static_cast<f64>(this->x - rhs.x),
		static_cast<f64>(this->y - rhs.y)
	};
}

Vector2_f64 Vector2_f64::operator * (Vector2_f64 rhs) {
	return Vector2_f64 {
		static_cast<f64>(this->x * rhs.x),
		static_cast<f64>(this->y * rhs.y)
	};
}

Vector2_f64 Vector2_f64::operator / (Vector2_f64 rhs) {
	return Vector2_f64 {
		static_cast<f64>(this->x / rhs.x),
		static_cast<f64>(this->y / rhs.y)
	};
}

Vector2_f64 Vector2_f64::operator + (f64 rhs)  {
	return Vector2_f64 {
		static_cast<f64>(this->x + rhs),
		static_cast<f64>(this->y + rhs)
	};
}

Vector2_f64 Vector2_f64::operator - (f64 rhs) {
	return Vector2_f64 {
		static_cast<f64>(this->x - rhs),
		static_cast<f64>(this->y - rhs)
	};
}

Vector2_f64 Vector2_f64::operator * (f64 rhs) {
	return Vector2_f64 {
		static_cast<f64>(this->x * rhs),
		static_cast<f64>(this->y * rhs)
	};
}

Vector2_f64 Vector2_f64::operator / (f64 rhs) {
	return Vector2_f64 {
		static_cast<f64>(this->x / rhs),
		static_cast<f64>(this->y / rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector3_s8
//-----------------------------------------------------------------------------

bool Vector3_s8::operator == (Vector3_s8 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y &&
	       this->z == rhs.z;
}

bool Vector3_s8::operator == (s8 rhs) {
	return this->x == rhs &&
	       this->y == rhs &&
	       this->z == rhs;
}

s8 & Vector3_s8::operator [] (int index) {
	assert(index >= 0 && index < 3);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
		case 2:  return z;
	}
}

Vector3_s8 & Vector3_s8::operator += (Vector3_s8 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	return *this;
}

Vector3_s8 & Vector3_s8::operator -= (Vector3_s8 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	return *this;
}

Vector3_s8 & Vector3_s8::operator *= (Vector3_s8 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	return *this;
}

Vector3_s8 & Vector3_s8::operator /= (Vector3_s8 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	return *this;
}

Vector3_s8 & Vector3_s8::operator %= (Vector3_s8 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	this->z %= rhs.z;
	return *this;
}

Vector3_s8 & Vector3_s8::operator += (s8 rhs) {
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	return *this;
}

Vector3_s8 & Vector3_s8::operator -= (s8 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	return *this;
}

Vector3_s8 & Vector3_s8::operator *= (s8 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	return *this;
}

Vector3_s8 & Vector3_s8::operator /= (s8 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	return *this;
}

Vector3_s8 & Vector3_s8::operator %= (s8 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	this->z %= rhs;
	return *this;
}

Vector3_s8 Vector3_s8::operator + (Vector3_s8 rhs) {
	return Vector3_s8 {
		static_cast<s8>(this->x + rhs.x),
		static_cast<s8>(this->y + rhs.y),
		static_cast<s8>(this->z + rhs.z)
	};
}

Vector3_s8 Vector3_s8::operator - (Vector3_s8 rhs) {
	return Vector3_s8 {
		static_cast<s8>(this->x - rhs.x),
		static_cast<s8>(this->y - rhs.y),
		static_cast<s8>(this->z - rhs.z)
	};
}

Vector3_s8 Vector3_s8::operator * (Vector3_s8 rhs) {
	return Vector3_s8 {
		static_cast<s8>(this->x * rhs.x),
		static_cast<s8>(this->y * rhs.y),
		static_cast<s8>(this->z * rhs.z)
	};
}

Vector3_s8 Vector3_s8::operator / (Vector3_s8 rhs) {
	return Vector3_s8 {
		static_cast<s8>(this->x / rhs.x),
		static_cast<s8>(this->y / rhs.y),
		static_cast<s8>(this->z / rhs.z)
	};
}

Vector3_s8 Vector3_s8::operator % (Vector3_s8 rhs) {
	return Vector3_s8 {
		static_cast<s8>(this->x % rhs.x),
		static_cast<s8>(this->y % rhs.y),
		static_cast<s8>(this->z % rhs.z)
	};
}

Vector3_s8 Vector3_s8::operator + (s8 rhs)  {
	return Vector3_s8 {
		static_cast<s8>(this->x + rhs),
		static_cast<s8>(this->y + rhs),
		static_cast<s8>(this->z + rhs)
	};
}

Vector3_s8 Vector3_s8::operator - (s8 rhs) {
	return Vector3_s8 {
		static_cast<s8>(this->x - rhs),
		static_cast<s8>(this->y - rhs),
		static_cast<s8>(this->z - rhs)
	};
}

Vector3_s8 Vector3_s8::operator * (s8 rhs) {
	return Vector3_s8 {
		static_cast<s8>(this->x * rhs),
		static_cast<s8>(this->y * rhs),
		static_cast<s8>(this->z * rhs)
	};
}

Vector3_s8 Vector3_s8::operator / (s8 rhs) {
	return Vector3_s8 {
		static_cast<s8>(this->x / rhs),
		static_cast<s8>(this->y / rhs),
		static_cast<s8>(this->z / rhs)
	};
}

Vector3_s8 Vector3_s8::operator % (s8 rhs) {
	return Vector3_s8 {
		static_cast<s8>(this->x % rhs),
		static_cast<s8>(this->y % rhs),
		static_cast<s8>(this->z % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector3_u8
//-----------------------------------------------------------------------------

bool Vector3_u8::operator == (Vector3_u8 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y &&
	       this->z == rhs.z;
}

bool Vector3_u8::operator == (u8 rhs) {
	return this->x == rhs &&
	       this->y == rhs &&
	       this->z == rhs;
}

u8 & Vector3_u8::operator [] (int index) {
	assert(index >= 0 && index < 3);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
		case 2:  return z;
	}
}

Vector3_u8 & Vector3_u8::operator += (Vector3_u8 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	return *this;
}

Vector3_u8 & Vector3_u8::operator -= (Vector3_u8 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	return *this;
}

Vector3_u8 & Vector3_u8::operator *= (Vector3_u8 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	return *this;
}

Vector3_u8 & Vector3_u8::operator /= (Vector3_u8 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	return *this;
}

Vector3_u8 & Vector3_u8::operator %= (Vector3_u8 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	this->z %= rhs.z;
	return *this;
}

Vector3_u8 & Vector3_u8::operator += (u8 rhs) {
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	return *this;
}

Vector3_u8 & Vector3_u8::operator -= (u8 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	return *this;
}

Vector3_u8 & Vector3_u8::operator *= (u8 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	return *this;
}

Vector3_u8 & Vector3_u8::operator /= (u8 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	return *this;
}

Vector3_u8 & Vector3_u8::operator %= (u8 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	this->z %= rhs;
	return *this;
}

Vector3_u8 Vector3_u8::operator + (Vector3_u8 rhs) {
	return Vector3_u8 {
		static_cast<u8>(this->x + rhs.x),
		static_cast<u8>(this->y + rhs.y),
		static_cast<u8>(this->z + rhs.z)
	};
}

Vector3_u8 Vector3_u8::operator - (Vector3_u8 rhs) {
	return Vector3_u8 {
		static_cast<u8>(this->x - rhs.x),
		static_cast<u8>(this->y - rhs.y),
		static_cast<u8>(this->z - rhs.z)
	};
}

Vector3_u8 Vector3_u8::operator * (Vector3_u8 rhs) {
	return Vector3_u8 {
		static_cast<u8>(this->x * rhs.x),
		static_cast<u8>(this->y * rhs.y),
		static_cast<u8>(this->z * rhs.z)
	};
}

Vector3_u8 Vector3_u8::operator / (Vector3_u8 rhs) {
	return Vector3_u8 {
		static_cast<u8>(this->x / rhs.x),
		static_cast<u8>(this->y / rhs.y),
		static_cast<u8>(this->z / rhs.z)
	};
}

Vector3_u8 Vector3_u8::operator % (Vector3_u8 rhs) {
	return Vector3_u8 {
		static_cast<u8>(this->x % rhs.x),
		static_cast<u8>(this->y % rhs.y),
		static_cast<u8>(this->z % rhs.z)
	};
}

Vector3_u8 Vector3_u8::operator + (u8 rhs)  {
	return Vector3_u8 {
		static_cast<u8>(this->x + rhs),
		static_cast<u8>(this->y + rhs),
		static_cast<u8>(this->z + rhs)
	};
}

Vector3_u8 Vector3_u8::operator - (u8 rhs) {
	return Vector3_u8 {
		static_cast<u8>(this->x - rhs),
		static_cast<u8>(this->y - rhs),
		static_cast<u8>(this->z - rhs)
	};
}

Vector3_u8 Vector3_u8::operator * (u8 rhs) {
	return Vector3_u8 {
		static_cast<u8>(this->x * rhs),
		static_cast<u8>(this->y * rhs),
		static_cast<u8>(this->z * rhs)
	};
}

Vector3_u8 Vector3_u8::operator / (u8 rhs) {
	return Vector3_u8 {
		static_cast<u8>(this->x / rhs),
		static_cast<u8>(this->y / rhs),
		static_cast<u8>(this->z / rhs)
	};
}

Vector3_u8 Vector3_u8::operator % (u8 rhs) {
	return Vector3_u8 {
		static_cast<u8>(this->x % rhs),
		static_cast<u8>(this->y % rhs),
		static_cast<u8>(this->z % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector3_s16
//-----------------------------------------------------------------------------

bool Vector3_s16::operator == (Vector3_s16 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y &&
	       this->z == rhs.z;
}

bool Vector3_s16::operator == (s16 rhs) {
	return this->x == rhs &&
	       this->y == rhs &&
	       this->z == rhs;
}

s16 & Vector3_s16::operator [] (int index) {
	assert(index >= 0 && index < 3);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
		case 2:  return z;
	}
}

Vector3_s16 & Vector3_s16::operator += (Vector3_s16 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	return *this;
}

Vector3_s16 & Vector3_s16::operator -= (Vector3_s16 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	return *this;
}

Vector3_s16 & Vector3_s16::operator *= (Vector3_s16 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	return *this;
}

Vector3_s16 & Vector3_s16::operator /= (Vector3_s16 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	return *this;
}

Vector3_s16 & Vector3_s16::operator %= (Vector3_s16 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	this->z %= rhs.z;
	return *this;
}

Vector3_s16 & Vector3_s16::operator += (s16 rhs) {
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	return *this;
}

Vector3_s16 & Vector3_s16::operator -= (s16 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	return *this;
}

Vector3_s16 & Vector3_s16::operator *= (s16 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	return *this;
}

Vector3_s16 & Vector3_s16::operator /= (s16 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	return *this;
}

Vector3_s16 & Vector3_s16::operator %= (s16 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	this->z %= rhs;
	return *this;
}

Vector3_s16 Vector3_s16::operator + (Vector3_s16 rhs) {
	return Vector3_s16 {
		static_cast<s16>(this->x + rhs.x),
		static_cast<s16>(this->y + rhs.y),
		static_cast<s16>(this->z + rhs.z)
	};
}

Vector3_s16 Vector3_s16::operator - (Vector3_s16 rhs) {
	return Vector3_s16 {
		static_cast<s16>(this->x - rhs.x),
		static_cast<s16>(this->y - rhs.y),
		static_cast<s16>(this->z - rhs.z)
	};
}

Vector3_s16 Vector3_s16::operator * (Vector3_s16 rhs) {
	return Vector3_s16 {
		static_cast<s16>(this->x * rhs.x),
		static_cast<s16>(this->y * rhs.y),
		static_cast<s16>(this->z * rhs.z)
	};
}

Vector3_s16 Vector3_s16::operator / (Vector3_s16 rhs) {
	return Vector3_s16 {
		static_cast<s16>(this->x / rhs.x),
		static_cast<s16>(this->y / rhs.y),
		static_cast<s16>(this->z / rhs.z)
	};
}

Vector3_s16 Vector3_s16::operator % (Vector3_s16 rhs) {
	return Vector3_s16 {
		static_cast<s16>(this->x % rhs.x),
		static_cast<s16>(this->y % rhs.y),
		static_cast<s16>(this->z % rhs.z)
	};
}

Vector3_s16 Vector3_s16::operator + (s16 rhs)  {
	return Vector3_s16 {
		static_cast<s16>(this->x + rhs),
		static_cast<s16>(this->y + rhs),
		static_cast<s16>(this->z + rhs)
	};
}

Vector3_s16 Vector3_s16::operator - (s16 rhs) {
	return Vector3_s16 {
		static_cast<s16>(this->x - rhs),
		static_cast<s16>(this->y - rhs),
		static_cast<s16>(this->z - rhs)
	};
}

Vector3_s16 Vector3_s16::operator * (s16 rhs) {
	return Vector3_s16 {
		static_cast<s16>(this->x * rhs),
		static_cast<s16>(this->y * rhs),
		static_cast<s16>(this->z * rhs)
	};
}

Vector3_s16 Vector3_s16::operator / (s16 rhs) {
	return Vector3_s16 {
		static_cast<s16>(this->x / rhs),
		static_cast<s16>(this->y / rhs),
		static_cast<s16>(this->z / rhs)
	};
}

Vector3_s16 Vector3_s16::operator % (s16 rhs) {
	return Vector3_s16 {
		static_cast<s16>(this->x % rhs),
		static_cast<s16>(this->y % rhs),
		static_cast<s16>(this->z % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector3_u16
//-----------------------------------------------------------------------------

bool Vector3_u16::operator == (Vector3_u16 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y &&
	       this->z == rhs.z;
}

bool Vector3_u16::operator == (u16 rhs) {
	return this->x == rhs &&
	       this->y == rhs &&
	       this->z == rhs;
}

u16 & Vector3_u16::operator [] (int index) {
	assert(index >= 0 && index < 3);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
		case 2:  return z;
	}
}

Vector3_u16 & Vector3_u16::operator += (Vector3_u16 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	return *this;
}

Vector3_u16 & Vector3_u16::operator -= (Vector3_u16 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	return *this;
}

Vector3_u16 & Vector3_u16::operator *= (Vector3_u16 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	return *this;
}

Vector3_u16 & Vector3_u16::operator /= (Vector3_u16 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	return *this;
}

Vector3_u16 & Vector3_u16::operator %= (Vector3_u16 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	this->z %= rhs.z;
	return *this;
}

Vector3_u16 & Vector3_u16::operator += (u16 rhs) {
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	return *this;
}

Vector3_u16 & Vector3_u16::operator -= (u16 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	return *this;
}

Vector3_u16 & Vector3_u16::operator *= (u16 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	return *this;
}

Vector3_u16 & Vector3_u16::operator /= (u16 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	return *this;
}

Vector3_u16 & Vector3_u16::operator %= (u16 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	this->z %= rhs;
	return *this;
}

Vector3_u16 Vector3_u16::operator + (Vector3_u16 rhs) {
	return Vector3_u16 {
		static_cast<u16>(this->x + rhs.x),
		static_cast<u16>(this->y + rhs.y),
		static_cast<u16>(this->z + rhs.z)
	};
}

Vector3_u16 Vector3_u16::operator - (Vector3_u16 rhs) {
	return Vector3_u16 {
		static_cast<u16>(this->x - rhs.x),
		static_cast<u16>(this->y - rhs.y),
		static_cast<u16>(this->z - rhs.z)
	};
}

Vector3_u16 Vector3_u16::operator * (Vector3_u16 rhs) {
	return Vector3_u16 {
		static_cast<u16>(this->x * rhs.x),
		static_cast<u16>(this->y * rhs.y),
		static_cast<u16>(this->z * rhs.z)
	};
}

Vector3_u16 Vector3_u16::operator / (Vector3_u16 rhs) {
	return Vector3_u16 {
		static_cast<u16>(this->x / rhs.x),
		static_cast<u16>(this->y / rhs.y),
		static_cast<u16>(this->z / rhs.z)
	};
}

Vector3_u16 Vector3_u16::operator % (Vector3_u16 rhs) {
	return Vector3_u16 {
		static_cast<u16>(this->x % rhs.x),
		static_cast<u16>(this->y % rhs.y),
		static_cast<u16>(this->z % rhs.z)
	};
}

Vector3_u16 Vector3_u16::operator + (u16 rhs)  {
	return Vector3_u16 {
		static_cast<u16>(this->x + rhs),
		static_cast<u16>(this->y + rhs),
		static_cast<u16>(this->z + rhs)
	};
}

Vector3_u16 Vector3_u16::operator - (u16 rhs) {
	return Vector3_u16 {
		static_cast<u16>(this->x - rhs),
		static_cast<u16>(this->y - rhs),
		static_cast<u16>(this->z - rhs)
	};
}

Vector3_u16 Vector3_u16::operator * (u16 rhs) {
	return Vector3_u16 {
		static_cast<u16>(this->x * rhs),
		static_cast<u16>(this->y * rhs),
		static_cast<u16>(this->z * rhs)
	};
}

Vector3_u16 Vector3_u16::operator / (u16 rhs) {
	return Vector3_u16 {
		static_cast<u16>(this->x / rhs),
		static_cast<u16>(this->y / rhs),
		static_cast<u16>(this->z / rhs)
	};
}

Vector3_u16 Vector3_u16::operator % (u16 rhs) {
	return Vector3_u16 {
		static_cast<u16>(this->x % rhs),
		static_cast<u16>(this->y % rhs),
		static_cast<u16>(this->z % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector3_s32
//-----------------------------------------------------------------------------

bool Vector3_s32::operator == (Vector3_s32 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y &&
	       this->z == rhs.z;
}

bool Vector3_s32::operator == (s32 rhs) {
	return this->x == rhs &&
	       this->y == rhs &&
	       this->z == rhs;
}

s32 & Vector3_s32::operator [] (int index) {
	assert(index >= 0 && index < 3);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
		case 2:  return z;
	}
}

Vector3_s32 & Vector3_s32::operator += (Vector3_s32 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	return *this;
}

Vector3_s32 & Vector3_s32::operator -= (Vector3_s32 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	return *this;
}

Vector3_s32 & Vector3_s32::operator *= (Vector3_s32 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	return *this;
}

Vector3_s32 & Vector3_s32::operator /= (Vector3_s32 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	return *this;
}

Vector3_s32 & Vector3_s32::operator %= (Vector3_s32 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	this->z %= rhs.z;
	return *this;
}

Vector3_s32 & Vector3_s32::operator += (s32 rhs) {
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	return *this;
}

Vector3_s32 & Vector3_s32::operator -= (s32 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	return *this;
}

Vector3_s32 & Vector3_s32::operator *= (s32 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	return *this;
}

Vector3_s32 & Vector3_s32::operator /= (s32 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	return *this;
}

Vector3_s32 & Vector3_s32::operator %= (s32 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	this->z %= rhs;
	return *this;
}

Vector3_s32 Vector3_s32::operator + (Vector3_s32 rhs) {
	return Vector3_s32 {
		static_cast<s32>(this->x + rhs.x),
		static_cast<s32>(this->y + rhs.y),
		static_cast<s32>(this->z + rhs.z)
	};
}

Vector3_s32 Vector3_s32::operator - (Vector3_s32 rhs) {
	return Vector3_s32 {
		static_cast<s32>(this->x - rhs.x),
		static_cast<s32>(this->y - rhs.y),
		static_cast<s32>(this->z - rhs.z)
	};
}

Vector3_s32 Vector3_s32::operator * (Vector3_s32 rhs) {
	return Vector3_s32 {
		static_cast<s32>(this->x * rhs.x),
		static_cast<s32>(this->y * rhs.y),
		static_cast<s32>(this->z * rhs.z)
	};
}

Vector3_s32 Vector3_s32::operator / (Vector3_s32 rhs) {
	return Vector3_s32 {
		static_cast<s32>(this->x / rhs.x),
		static_cast<s32>(this->y / rhs.y),
		static_cast<s32>(this->z / rhs.z)
	};
}

Vector3_s32 Vector3_s32::operator % (Vector3_s32 rhs) {
	return Vector3_s32 {
		static_cast<s32>(this->x % rhs.x),
		static_cast<s32>(this->y % rhs.y),
		static_cast<s32>(this->z % rhs.z)
	};
}

Vector3_s32 Vector3_s32::operator + (s32 rhs)  {
	return Vector3_s32 {
		static_cast<s32>(this->x + rhs),
		static_cast<s32>(this->y + rhs),
		static_cast<s32>(this->z + rhs)
	};
}

Vector3_s32 Vector3_s32::operator - (s32 rhs) {
	return Vector3_s32 {
		static_cast<s32>(this->x - rhs),
		static_cast<s32>(this->y - rhs),
		static_cast<s32>(this->z - rhs)
	};
}

Vector3_s32 Vector3_s32::operator * (s32 rhs) {
	return Vector3_s32 {
		static_cast<s32>(this->x * rhs),
		static_cast<s32>(this->y * rhs),
		static_cast<s32>(this->z * rhs)
	};
}

Vector3_s32 Vector3_s32::operator / (s32 rhs) {
	return Vector3_s32 {
		static_cast<s32>(this->x / rhs),
		static_cast<s32>(this->y / rhs),
		static_cast<s32>(this->z / rhs)
	};
}

Vector3_s32 Vector3_s32::operator % (s32 rhs) {
	return Vector3_s32 {
		static_cast<s32>(this->x % rhs),
		static_cast<s32>(this->y % rhs),
		static_cast<s32>(this->z % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector3_u32
//-----------------------------------------------------------------------------

bool Vector3_u32::operator == (Vector3_u32 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y &&
	       this->z == rhs.z;
}

bool Vector3_u32::operator == (u32 rhs) {
	return this->x == rhs &&
	       this->y == rhs &&
	       this->z == rhs;
}

u32 & Vector3_u32::operator [] (int index) {
	assert(index >= 0 && index < 3);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
		case 2:  return z;
	}
}

Vector3_u32 & Vector3_u32::operator += (Vector3_u32 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	return *this;
}

Vector3_u32 & Vector3_u32::operator -= (Vector3_u32 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	return *this;
}

Vector3_u32 & Vector3_u32::operator *= (Vector3_u32 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	return *this;
}

Vector3_u32 & Vector3_u32::operator /= (Vector3_u32 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	return *this;
}

Vector3_u32 & Vector3_u32::operator %= (Vector3_u32 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	this->z %= rhs.z;
	return *this;
}

Vector3_u32 & Vector3_u32::operator += (u32 rhs) {
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	return *this;
}

Vector3_u32 & Vector3_u32::operator -= (u32 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	return *this;
}

Vector3_u32 & Vector3_u32::operator *= (u32 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	return *this;
}

Vector3_u32 & Vector3_u32::operator /= (u32 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	return *this;
}

Vector3_u32 & Vector3_u32::operator %= (u32 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	this->z %= rhs;
	return *this;
}

Vector3_u32 Vector3_u32::operator + (Vector3_u32 rhs) {
	return Vector3_u32 {
		static_cast<u32>(this->x + rhs.x),
		static_cast<u32>(this->y + rhs.y),
		static_cast<u32>(this->z + rhs.z)
	};
}

Vector3_u32 Vector3_u32::operator - (Vector3_u32 rhs) {
	return Vector3_u32 {
		static_cast<u32>(this->x - rhs.x),
		static_cast<u32>(this->y - rhs.y),
		static_cast<u32>(this->z - rhs.z)
	};
}

Vector3_u32 Vector3_u32::operator * (Vector3_u32 rhs) {
	return Vector3_u32 {
		static_cast<u32>(this->x * rhs.x),
		static_cast<u32>(this->y * rhs.y),
		static_cast<u32>(this->z * rhs.z)
	};
}

Vector3_u32 Vector3_u32::operator / (Vector3_u32 rhs) {
	return Vector3_u32 {
		static_cast<u32>(this->x / rhs.x),
		static_cast<u32>(this->y / rhs.y),
		static_cast<u32>(this->z / rhs.z)
	};
}

Vector3_u32 Vector3_u32::operator % (Vector3_u32 rhs) {
	return Vector3_u32 {
		static_cast<u32>(this->x % rhs.x),
		static_cast<u32>(this->y % rhs.y),
		static_cast<u32>(this->z % rhs.z)
	};
}

Vector3_u32 Vector3_u32::operator + (u32 rhs)  {
	return Vector3_u32 {
		static_cast<u32>(this->x + rhs),
		static_cast<u32>(this->y + rhs),
		static_cast<u32>(this->z + rhs)
	};
}

Vector3_u32 Vector3_u32::operator - (u32 rhs) {
	return Vector3_u32 {
		static_cast<u32>(this->x - rhs),
		static_cast<u32>(this->y - rhs),
		static_cast<u32>(this->z - rhs)
	};
}

Vector3_u32 Vector3_u32::operator * (u32 rhs) {
	return Vector3_u32 {
		static_cast<u32>(this->x * rhs),
		static_cast<u32>(this->y * rhs),
		static_cast<u32>(this->z * rhs)
	};
}

Vector3_u32 Vector3_u32::operator / (u32 rhs) {
	return Vector3_u32 {
		static_cast<u32>(this->x / rhs),
		static_cast<u32>(this->y / rhs),
		static_cast<u32>(this->z / rhs)
	};
}

Vector3_u32 Vector3_u32::operator % (u32 rhs) {
	return Vector3_u32 {
		static_cast<u32>(this->x % rhs),
		static_cast<u32>(this->y % rhs),
		static_cast<u32>(this->z % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector3_s64
//-----------------------------------------------------------------------------

bool Vector3_s64::operator == (Vector3_s64 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y &&
	       this->z == rhs.z;
}

bool Vector3_s64::operator == (s64 rhs) {
	return this->x == rhs &&
	       this->y == rhs &&
	       this->z == rhs;
}

s64 & Vector3_s64::operator [] (int index) {
	assert(index >= 0 && index < 3);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
		case 2:  return z;
	}
}

Vector3_s64 & Vector3_s64::operator += (Vector3_s64 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	return *this;
}

Vector3_s64 & Vector3_s64::operator -= (Vector3_s64 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	return *this;
}

Vector3_s64 & Vector3_s64::operator *= (Vector3_s64 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	return *this;
}

Vector3_s64 & Vector3_s64::operator /= (Vector3_s64 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	return *this;
}

Vector3_s64 & Vector3_s64::operator %= (Vector3_s64 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	this->z %= rhs.z;
	return *this;
}

Vector3_s64 & Vector3_s64::operator += (s64 rhs) {
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	return *this;
}

Vector3_s64 & Vector3_s64::operator -= (s64 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	return *this;
}

Vector3_s64 & Vector3_s64::operator *= (s64 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	return *this;
}

Vector3_s64 & Vector3_s64::operator /= (s64 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	return *this;
}

Vector3_s64 & Vector3_s64::operator %= (s64 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	this->z %= rhs;
	return *this;
}

Vector3_s64 Vector3_s64::operator + (Vector3_s64 rhs) {
	return Vector3_s64 {
		static_cast<s64>(this->x + rhs.x),
		static_cast<s64>(this->y + rhs.y),
		static_cast<s64>(this->z + rhs.z)
	};
}

Vector3_s64 Vector3_s64::operator - (Vector3_s64 rhs) {
	return Vector3_s64 {
		static_cast<s64>(this->x - rhs.x),
		static_cast<s64>(this->y - rhs.y),
		static_cast<s64>(this->z - rhs.z)
	};
}

Vector3_s64 Vector3_s64::operator * (Vector3_s64 rhs) {
	return Vector3_s64 {
		static_cast<s64>(this->x * rhs.x),
		static_cast<s64>(this->y * rhs.y),
		static_cast<s64>(this->z * rhs.z)
	};
}

Vector3_s64 Vector3_s64::operator / (Vector3_s64 rhs) {
	return Vector3_s64 {
		static_cast<s64>(this->x / rhs.x),
		static_cast<s64>(this->y / rhs.y),
		static_cast<s64>(this->z / rhs.z)
	};
}

Vector3_s64 Vector3_s64::operator % (Vector3_s64 rhs) {
	return Vector3_s64 {
		static_cast<s64>(this->x % rhs.x),
		static_cast<s64>(this->y % rhs.y),
		static_cast<s64>(this->z % rhs.z)
	};
}

Vector3_s64 Vector3_s64::operator + (s64 rhs)  {
	return Vector3_s64 {
		static_cast<s64>(this->x + rhs),
		static_cast<s64>(this->y + rhs),
		static_cast<s64>(this->z + rhs)
	};
}

Vector3_s64 Vector3_s64::operator - (s64 rhs) {
	return Vector3_s64 {
		static_cast<s64>(this->x - rhs),
		static_cast<s64>(this->y - rhs),
		static_cast<s64>(this->z - rhs)
	};
}

Vector3_s64 Vector3_s64::operator * (s64 rhs) {
	return Vector3_s64 {
		static_cast<s64>(this->x * rhs),
		static_cast<s64>(this->y * rhs),
		static_cast<s64>(this->z * rhs)
	};
}

Vector3_s64 Vector3_s64::operator / (s64 rhs) {
	return Vector3_s64 {
		static_cast<s64>(this->x / rhs),
		static_cast<s64>(this->y / rhs),
		static_cast<s64>(this->z / rhs)
	};
}

Vector3_s64 Vector3_s64::operator % (s64 rhs) {
	return Vector3_s64 {
		static_cast<s64>(this->x % rhs),
		static_cast<s64>(this->y % rhs),
		static_cast<s64>(this->z % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector3_u64
//-----------------------------------------------------------------------------

bool Vector3_u64::operator == (Vector3_u64 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y &&
	       this->z == rhs.z;
}

bool Vector3_u64::operator == (u64 rhs) {
	return this->x == rhs &&
	       this->y == rhs &&
	       this->z == rhs;
}

u64 & Vector3_u64::operator [] (int index) {
	assert(index >= 0 && index < 3);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
		case 2:  return z;
	}
}

Vector3_u64 & Vector3_u64::operator += (Vector3_u64 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	return *this;
}

Vector3_u64 & Vector3_u64::operator -= (Vector3_u64 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	return *this;
}

Vector3_u64 & Vector3_u64::operator *= (Vector3_u64 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	return *this;
}

Vector3_u64 & Vector3_u64::operator /= (Vector3_u64 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	return *this;
}

Vector3_u64 & Vector3_u64::operator %= (Vector3_u64 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	this->z %= rhs.z;
	return *this;
}

Vector3_u64 & Vector3_u64::operator += (u64 rhs) {
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	return *this;
}

Vector3_u64 & Vector3_u64::operator -= (u64 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	return *this;
}

Vector3_u64 & Vector3_u64::operator *= (u64 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	return *this;
}

Vector3_u64 & Vector3_u64::operator /= (u64 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	return *this;
}

Vector3_u64 & Vector3_u64::operator %= (u64 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	this->z %= rhs;
	return *this;
}

Vector3_u64 Vector3_u64::operator + (Vector3_u64 rhs) {
	return Vector3_u64 {
		static_cast<u64>(this->x + rhs.x),
		static_cast<u64>(this->y + rhs.y),
		static_cast<u64>(this->z + rhs.z)
	};
}

Vector3_u64 Vector3_u64::operator - (Vector3_u64 rhs) {
	return Vector3_u64 {
		static_cast<u64>(this->x - rhs.x),
		static_cast<u64>(this->y - rhs.y),
		static_cast<u64>(this->z - rhs.z)
	};
}

Vector3_u64 Vector3_u64::operator * (Vector3_u64 rhs) {
	return Vector3_u64 {
		static_cast<u64>(this->x * rhs.x),
		static_cast<u64>(this->y * rhs.y),
		static_cast<u64>(this->z * rhs.z)
	};
}

Vector3_u64 Vector3_u64::operator / (Vector3_u64 rhs) {
	return Vector3_u64 {
		static_cast<u64>(this->x / rhs.x),
		static_cast<u64>(this->y / rhs.y),
		static_cast<u64>(this->z / rhs.z)
	};
}

Vector3_u64 Vector3_u64::operator % (Vector3_u64 rhs) {
	return Vector3_u64 {
		static_cast<u64>(this->x % rhs.x),
		static_cast<u64>(this->y % rhs.y),
		static_cast<u64>(this->z % rhs.z)
	};
}

Vector3_u64 Vector3_u64::operator + (u64 rhs)  {
	return Vector3_u64 {
		static_cast<u64>(this->x + rhs),
		static_cast<u64>(this->y + rhs),
		static_cast<u64>(this->z + rhs)
	};
}

Vector3_u64 Vector3_u64::operator - (u64 rhs) {
	return Vector3_u64 {
		static_cast<u64>(this->x - rhs),
		static_cast<u64>(this->y - rhs),
		static_cast<u64>(this->z - rhs)
	};
}

Vector3_u64 Vector3_u64::operator * (u64 rhs) {
	return Vector3_u64 {
		static_cast<u64>(this->x * rhs),
		static_cast<u64>(this->y * rhs),
		static_cast<u64>(this->z * rhs)
	};
}

Vector3_u64 Vector3_u64::operator / (u64 rhs) {
	return Vector3_u64 {
		static_cast<u64>(this->x / rhs),
		static_cast<u64>(this->y / rhs),
		static_cast<u64>(this->z / rhs)
	};
}

Vector3_u64 Vector3_u64::operator % (u64 rhs) {
	return Vector3_u64 {
		static_cast<u64>(this->x % rhs),
		static_cast<u64>(this->y % rhs),
		static_cast<u64>(this->z % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector3_f32
//-----------------------------------------------------------------------------

bool Vector3_f32::operator == (Vector3_f32 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y &&
	       this->z == rhs.z;
}

bool Vector3_f32::operator == (f32 rhs) {
	return this->x == rhs &&
	       this->y == rhs &&
	       this->z == rhs;
}

f32 & Vector3_f32::operator [] (int index) {
	assert(index >= 0 && index < 3);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
		case 2:  return z;
	}
}

Vector3_f32 & Vector3_f32::operator += (Vector3_f32 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	return *this;
}

Vector3_f32 & Vector3_f32::operator -= (Vector3_f32 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	return *this;
}

Vector3_f32 & Vector3_f32::operator *= (Vector3_f32 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	return *this;
}

Vector3_f32 & Vector3_f32::operator /= (Vector3_f32 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	return *this;
}

Vector3_f32 & Vector3_f32::operator += (f32 rhs) {
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	return *this;
}

Vector3_f32 & Vector3_f32::operator -= (f32 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	return *this;
}

Vector3_f32 & Vector3_f32::operator *= (f32 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	return *this;
}

Vector3_f32 & Vector3_f32::operator /= (f32 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	return *this;
}

Vector3_f32 Vector3_f32::operator + (Vector3_f32 rhs) {
	return Vector3_f32 {
		static_cast<f32>(this->x + rhs.x),
		static_cast<f32>(this->y + rhs.y),
		static_cast<f32>(this->z + rhs.z)
	};
}

Vector3_f32 Vector3_f32::operator - (Vector3_f32 rhs) {
	return Vector3_f32 {
		static_cast<f32>(this->x - rhs.x),
		static_cast<f32>(this->y - rhs.y),
		static_cast<f32>(this->z - rhs.z)
	};
}

Vector3_f32 Vector3_f32::operator * (Vector3_f32 rhs) {
	return Vector3_f32 {
		static_cast<f32>(this->x * rhs.x),
		static_cast<f32>(this->y * rhs.y),
		static_cast<f32>(this->z * rhs.z)
	};
}

Vector3_f32 Vector3_f32::operator / (Vector3_f32 rhs) {
	return Vector3_f32 {
		static_cast<f32>(this->x / rhs.x),
		static_cast<f32>(this->y / rhs.y),
		static_cast<f32>(this->z / rhs.z)
	};
}

Vector3_f32 Vector3_f32::operator + (f32 rhs)  {
	return Vector3_f32 {
		static_cast<f32>(this->x + rhs),
		static_cast<f32>(this->y + rhs),
		static_cast<f32>(this->z + rhs)
	};
}

Vector3_f32 Vector3_f32::operator - (f32 rhs) {
	return Vector3_f32 {
		static_cast<f32>(this->x - rhs),
		static_cast<f32>(this->y - rhs),
		static_cast<f32>(this->z - rhs)
	};
}

Vector3_f32 Vector3_f32::operator * (f32 rhs) {
	return Vector3_f32 {
		static_cast<f32>(this->x * rhs),
		static_cast<f32>(this->y * rhs),
		static_cast<f32>(this->z * rhs)
	};
}

Vector3_f32 Vector3_f32::operator / (f32 rhs) {
	return Vector3_f32 {
		static_cast<f32>(this->x / rhs),
		static_cast<f32>(this->y / rhs),
		static_cast<f32>(this->z / rhs)
	};
}

Vector3_f32 Vector3_f32::operator - () {
	return Vector3_f32 {
		static_cast<f32>(-this->x),
		static_cast<f32>(-this->y),
		static_cast<f32>(-this->z)
	};
}

Vector3_f32 Vector3_f32::operator - () const {
	return Vector3_f32 {
		static_cast<f32>(-this->x),
		static_cast<f32>(-this->y),
		static_cast<f32>(-this->z)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector3_f64
//-----------------------------------------------------------------------------

bool Vector3_f64::operator == (Vector3_f64 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y &&
	       this->z == rhs.z;
}

bool Vector3_f64::operator == (f64 rhs) {
	return this->x == rhs &&
	       this->y == rhs &&
	       this->z == rhs;
}

f64 & Vector3_f64::operator [] (int index) {
	assert(index >= 0 && index < 3);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
		case 2:  return z;
	}
}

Vector3_f64 & Vector3_f64::operator += (Vector3_f64 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	return *this;
}

Vector3_f64 & Vector3_f64::operator -= (Vector3_f64 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	return *this;
}

Vector3_f64 & Vector3_f64::operator *= (Vector3_f64 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	return *this;
}

Vector3_f64 & Vector3_f64::operator /= (Vector3_f64 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	return *this;
}

Vector3_f64 & Vector3_f64::operator += (f64 rhs) {
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	return *this;
}

Vector3_f64 & Vector3_f64::operator -= (f64 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	return *this;
}

Vector3_f64 & Vector3_f64::operator *= (f64 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	return *this;
}

Vector3_f64 & Vector3_f64::operator /= (f64 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	return *this;
}

Vector3_f64 Vector3_f64::operator + (Vector3_f64 rhs) {
	return Vector3_f64 {
		static_cast<f64>(this->x + rhs.x),
		static_cast<f64>(this->y + rhs.y),
		static_cast<f64>(this->z + rhs.z)
	};
}

Vector3_f64 Vector3_f64::operator - (Vector3_f64 rhs) {
	return Vector3_f64 {
		static_cast<f64>(this->x - rhs.x),
		static_cast<f64>(this->y - rhs.y),
		static_cast<f64>(this->z - rhs.z)
	};
}

Vector3_f64 Vector3_f64::operator * (Vector3_f64 rhs) {
	return Vector3_f64 {
		static_cast<f64>(this->x * rhs.x),
		static_cast<f64>(this->y * rhs.y),
		static_cast<f64>(this->z * rhs.z)
	};
}

Vector3_f64 Vector3_f64::operator / (Vector3_f64 rhs) {
	return Vector3_f64 {
		static_cast<f64>(this->x / rhs.x),
		static_cast<f64>(this->y / rhs.y),
		static_cast<f64>(this->z / rhs.z)
	};
}

Vector3_f64 Vector3_f64::operator + (f64 rhs)  {
	return Vector3_f64 {
		static_cast<f64>(this->x + rhs),
		static_cast<f64>(this->y + rhs),
		static_cast<f64>(this->z + rhs)
	};
}

Vector3_f64 Vector3_f64::operator - (f64 rhs) {
	return Vector3_f64 {
		static_cast<f64>(this->x - rhs),
		static_cast<f64>(this->y - rhs),
		static_cast<f64>(this->z - rhs)
	};
}

Vector3_f64 Vector3_f64::operator * (f64 rhs) {
	return Vector3_f64 {
		static_cast<f64>(this->x * rhs),
		static_cast<f64>(this->y * rhs),
		static_cast<f64>(this->z * rhs)
	};
}

Vector3_f64 Vector3_f64::operator / (f64 rhs) {
	return Vector3_f64 {
		static_cast<f64>(this->x / rhs),
		static_cast<f64>(this->y / rhs),
		static_cast<f64>(this->z / rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector4_s8
//-----------------------------------------------------------------------------

bool Vector4_s8::operator == (Vector4_s8 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y &&
	       this->z == rhs.z &&
	       this->w == rhs.w;
}

bool Vector4_s8::operator == (s8 rhs) {
	return this->x == rhs &&
	       this->y == rhs &&
	       this->z == rhs &&
	       this->w == rhs;
}

s8 & Vector4_s8::operator [] (int index) {
	assert(index >= 0 && index < 4);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
		case 2:  return z;
		case 3:  return w;
	}
}

Vector4_s8 & Vector4_s8::operator += (Vector4_s8 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	this->w += rhs.w;
	return *this;
}

Vector4_s8 & Vector4_s8::operator -= (Vector4_s8 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	this->w -= rhs.w;
	return *this;
}

Vector4_s8 & Vector4_s8::operator *= (Vector4_s8 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	this->w *= rhs.w;
	return *this;
}

Vector4_s8 & Vector4_s8::operator /= (Vector4_s8 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	this->w /= rhs.w;
	return *this;
}

Vector4_s8 & Vector4_s8::operator %= (Vector4_s8 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	this->z %= rhs.z;
	this->w %= rhs.w;
	return *this;
}

Vector4_s8 & Vector4_s8::operator += (s8 rhs) {
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	this->w += rhs;
	return *this;
}

Vector4_s8 & Vector4_s8::operator -= (s8 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	this->w -= rhs;
	return *this;
}

Vector4_s8 & Vector4_s8::operator *= (s8 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	this->w *= rhs;
	return *this;
}

Vector4_s8 & Vector4_s8::operator /= (s8 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	this->w /= rhs;
	return *this;
}

Vector4_s8 & Vector4_s8::operator %= (s8 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	this->z %= rhs;
	this->w %= rhs;
	return *this;
}

Vector4_s8 Vector4_s8::operator + (Vector4_s8 rhs) {
	return Vector4_s8 {
		static_cast<s8>(this->x + rhs.x),
		static_cast<s8>(this->y + rhs.y),
		static_cast<s8>(this->z + rhs.z),
		static_cast<s8>(this->w + rhs.w)
	};
}

Vector4_s8 Vector4_s8::operator - (Vector4_s8 rhs) {
	return Vector4_s8 {
		static_cast<s8>(this->x - rhs.x),
		static_cast<s8>(this->y - rhs.y),
		static_cast<s8>(this->z - rhs.z),
		static_cast<s8>(this->w - rhs.w)
	};
}

Vector4_s8 Vector4_s8::operator * (Vector4_s8 rhs) {
	return Vector4_s8 {
		static_cast<s8>(this->x * rhs.x),
		static_cast<s8>(this->y * rhs.y),
		static_cast<s8>(this->z * rhs.z),
		static_cast<s8>(this->w * rhs.w)
	};
}

Vector4_s8 Vector4_s8::operator / (Vector4_s8 rhs) {
	return Vector4_s8 {
		static_cast<s8>(this->x / rhs.x),
		static_cast<s8>(this->y / rhs.y),
		static_cast<s8>(this->z / rhs.z),
		static_cast<s8>(this->w / rhs.w)
	};
}

Vector4_s8 Vector4_s8::operator % (Vector4_s8 rhs) {
	return Vector4_s8 {
		static_cast<s8>(this->x % rhs.x),
		static_cast<s8>(this->y % rhs.y),
		static_cast<s8>(this->z % rhs.z),
		static_cast<s8>(this->w % rhs.w)
	};
}

Vector4_s8 Vector4_s8::operator + (s8 rhs)  {
	return Vector4_s8 {
		static_cast<s8>(this->x + rhs),
		static_cast<s8>(this->y + rhs),
		static_cast<s8>(this->z + rhs),
		static_cast<s8>(this->w + rhs)
	};
}

Vector4_s8 Vector4_s8::operator - (s8 rhs) {
	return Vector4_s8 {
		static_cast<s8>(this->x - rhs),
		static_cast<s8>(this->y - rhs),
		static_cast<s8>(this->z - rhs),
		static_cast<s8>(this->w - rhs)
	};
}

Vector4_s8 Vector4_s8::operator * (s8 rhs) {
	return Vector4_s8 {
		static_cast<s8>(this->x * rhs),
		static_cast<s8>(this->y * rhs),
		static_cast<s8>(this->z * rhs),
		static_cast<s8>(this->w * rhs)
	};
}

Vector4_s8 Vector4_s8::operator / (s8 rhs) {
	return Vector4_s8 {
		static_cast<s8>(this->x / rhs),
		static_cast<s8>(this->y / rhs),
		static_cast<s8>(this->z / rhs),
		static_cast<s8>(this->w / rhs)
	};
}

Vector4_s8 Vector4_s8::operator % (s8 rhs) {
	return Vector4_s8 {
		static_cast<s8>(this->x % rhs),
		static_cast<s8>(this->y % rhs),
		static_cast<s8>(this->z % rhs),
		static_cast<s8>(this->w % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector4_u8
//-----------------------------------------------------------------------------

bool Vector4_u8::operator == (Vector4_u8 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y &&
	       this->z == rhs.z &&
	       this->w == rhs.w;
}

bool Vector4_u8::operator == (u8 rhs) {
	return this->x == rhs &&
	       this->y == rhs &&
	       this->z == rhs &&
	       this->w == rhs;
}

u8 & Vector4_u8::operator [] (int index) {
	assert(index >= 0 && index < 4);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
		case 2:  return z;
		case 3:  return w;
	}
}

Vector4_u8 & Vector4_u8::operator += (Vector4_u8 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	this->w += rhs.w;
	return *this;
}

Vector4_u8 & Vector4_u8::operator -= (Vector4_u8 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	this->w -= rhs.w;
	return *this;
}

Vector4_u8 & Vector4_u8::operator *= (Vector4_u8 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	this->w *= rhs.w;
	return *this;
}

Vector4_u8 & Vector4_u8::operator /= (Vector4_u8 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	this->w /= rhs.w;
	return *this;
}

Vector4_u8 & Vector4_u8::operator %= (Vector4_u8 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	this->z %= rhs.z;
	this->w %= rhs.w;
	return *this;
}

Vector4_u8 & Vector4_u8::operator += (u8 rhs) {
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	this->w += rhs;
	return *this;
}

Vector4_u8 & Vector4_u8::operator -= (u8 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	this->w -= rhs;
	return *this;
}

Vector4_u8 & Vector4_u8::operator *= (u8 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	this->w *= rhs;
	return *this;
}

Vector4_u8 & Vector4_u8::operator /= (u8 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	this->w /= rhs;
	return *this;
}

Vector4_u8 & Vector4_u8::operator %= (u8 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	this->z %= rhs;
	this->w %= rhs;
	return *this;
}

Vector4_u8 Vector4_u8::operator + (Vector4_u8 rhs) {
	return Vector4_u8 {
		static_cast<u8>(this->x + rhs.x),
		static_cast<u8>(this->y + rhs.y),
		static_cast<u8>(this->z + rhs.z),
		static_cast<u8>(this->w + rhs.w)
	};
}

Vector4_u8 Vector4_u8::operator - (Vector4_u8 rhs) {
	return Vector4_u8 {
		static_cast<u8>(this->x - rhs.x),
		static_cast<u8>(this->y - rhs.y),
		static_cast<u8>(this->z - rhs.z),
		static_cast<u8>(this->w - rhs.w)
	};
}

Vector4_u8 Vector4_u8::operator * (Vector4_u8 rhs) {
	return Vector4_u8 {
		static_cast<u8>(this->x * rhs.x),
		static_cast<u8>(this->y * rhs.y),
		static_cast<u8>(this->z * rhs.z),
		static_cast<u8>(this->w * rhs.w)
	};
}

Vector4_u8 Vector4_u8::operator / (Vector4_u8 rhs) {
	return Vector4_u8 {
		static_cast<u8>(this->x / rhs.x),
		static_cast<u8>(this->y / rhs.y),
		static_cast<u8>(this->z / rhs.z),
		static_cast<u8>(this->w / rhs.w)
	};
}

Vector4_u8 Vector4_u8::operator % (Vector4_u8 rhs) {
	return Vector4_u8 {
		static_cast<u8>(this->x % rhs.x),
		static_cast<u8>(this->y % rhs.y),
		static_cast<u8>(this->z % rhs.z),
		static_cast<u8>(this->w % rhs.w)
	};
}

Vector4_u8 Vector4_u8::operator + (u8 rhs)  {
	return Vector4_u8 {
		static_cast<u8>(this->x + rhs),
		static_cast<u8>(this->y + rhs),
		static_cast<u8>(this->z + rhs),
		static_cast<u8>(this->w + rhs)
	};
}

Vector4_u8 Vector4_u8::operator - (u8 rhs) {
	return Vector4_u8 {
		static_cast<u8>(this->x - rhs),
		static_cast<u8>(this->y - rhs),
		static_cast<u8>(this->z - rhs),
		static_cast<u8>(this->w - rhs)
	};
}

Vector4_u8 Vector4_u8::operator * (u8 rhs) {
	return Vector4_u8 {
		static_cast<u8>(this->x * rhs),
		static_cast<u8>(this->y * rhs),
		static_cast<u8>(this->z * rhs),
		static_cast<u8>(this->w * rhs)
	};
}

Vector4_u8 Vector4_u8::operator / (u8 rhs) {
	return Vector4_u8 {
		static_cast<u8>(this->x / rhs),
		static_cast<u8>(this->y / rhs),
		static_cast<u8>(this->z / rhs),
		static_cast<u8>(this->w / rhs)
	};
}

Vector4_u8 Vector4_u8::operator % (u8 rhs) {
	return Vector4_u8 {
		static_cast<u8>(this->x % rhs),
		static_cast<u8>(this->y % rhs),
		static_cast<u8>(this->z % rhs),
		static_cast<u8>(this->w % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector4_s16
//-----------------------------------------------------------------------------

bool Vector4_s16::operator == (Vector4_s16 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y &&
	       this->z == rhs.z &&
	       this->w == rhs.w;
}

bool Vector4_s16::operator == (s16 rhs) {
	return this->x == rhs &&
	       this->y == rhs &&
	       this->z == rhs &&
	       this->w == rhs;
}

s16 & Vector4_s16::operator [] (int index) {
	assert(index >= 0 && index < 4);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
		case 2:  return z;
		case 3:  return w;
	}
}

Vector4_s16 & Vector4_s16::operator += (Vector4_s16 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	this->w += rhs.w;
	return *this;
}

Vector4_s16 & Vector4_s16::operator -= (Vector4_s16 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	this->w -= rhs.w;
	return *this;
}

Vector4_s16 & Vector4_s16::operator *= (Vector4_s16 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	this->w *= rhs.w;
	return *this;
}

Vector4_s16 & Vector4_s16::operator /= (Vector4_s16 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	this->w /= rhs.w;
	return *this;
}

Vector4_s16 & Vector4_s16::operator %= (Vector4_s16 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	this->z %= rhs.z;
	this->w %= rhs.w;
	return *this;
}

Vector4_s16 & Vector4_s16::operator += (s16 rhs) {
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	this->w += rhs;
	return *this;
}

Vector4_s16 & Vector4_s16::operator -= (s16 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	this->w -= rhs;
	return *this;
}

Vector4_s16 & Vector4_s16::operator *= (s16 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	this->w *= rhs;
	return *this;
}

Vector4_s16 & Vector4_s16::operator /= (s16 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	this->w /= rhs;
	return *this;
}

Vector4_s16 & Vector4_s16::operator %= (s16 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	this->z %= rhs;
	this->w %= rhs;
	return *this;
}

Vector4_s16 Vector4_s16::operator + (Vector4_s16 rhs) {
	return Vector4_s16 {
		static_cast<s16>(this->x + rhs.x),
		static_cast<s16>(this->y + rhs.y),
		static_cast<s16>(this->z + rhs.z),
		static_cast<s16>(this->w + rhs.w)
	};
}

Vector4_s16 Vector4_s16::operator - (Vector4_s16 rhs) {
	return Vector4_s16 {
		static_cast<s16>(this->x - rhs.x),
		static_cast<s16>(this->y - rhs.y),
		static_cast<s16>(this->z - rhs.z),
		static_cast<s16>(this->w - rhs.w)
	};
}

Vector4_s16 Vector4_s16::operator * (Vector4_s16 rhs) {
	return Vector4_s16 {
		static_cast<s16>(this->x * rhs.x),
		static_cast<s16>(this->y * rhs.y),
		static_cast<s16>(this->z * rhs.z),
		static_cast<s16>(this->w * rhs.w)
	};
}

Vector4_s16 Vector4_s16::operator / (Vector4_s16 rhs) {
	return Vector4_s16 {
		static_cast<s16>(this->x / rhs.x),
		static_cast<s16>(this->y / rhs.y),
		static_cast<s16>(this->z / rhs.z),
		static_cast<s16>(this->w / rhs.w)
	};
}

Vector4_s16 Vector4_s16::operator % (Vector4_s16 rhs) {
	return Vector4_s16 {
		static_cast<s16>(this->x % rhs.x),
		static_cast<s16>(this->y % rhs.y),
		static_cast<s16>(this->z % rhs.z),
		static_cast<s16>(this->w % rhs.w)
	};
}

Vector4_s16 Vector4_s16::operator + (s16 rhs)  {
	return Vector4_s16 {
		static_cast<s16>(this->x + rhs),
		static_cast<s16>(this->y + rhs),
		static_cast<s16>(this->z + rhs),
		static_cast<s16>(this->w + rhs)
	};
}

Vector4_s16 Vector4_s16::operator - (s16 rhs) {
	return Vector4_s16 {
		static_cast<s16>(this->x - rhs),
		static_cast<s16>(this->y - rhs),
		static_cast<s16>(this->z - rhs),
		static_cast<s16>(this->w - rhs)
	};
}

Vector4_s16 Vector4_s16::operator * (s16 rhs) {
	return Vector4_s16 {
		static_cast<s16>(this->x * rhs),
		static_cast<s16>(this->y * rhs),
		static_cast<s16>(this->z * rhs),
		static_cast<s16>(this->w * rhs)
	};
}

Vector4_s16 Vector4_s16::operator / (s16 rhs) {
	return Vector4_s16 {
		static_cast<s16>(this->x / rhs),
		static_cast<s16>(this->y / rhs),
		static_cast<s16>(this->z / rhs),
		static_cast<s16>(this->w / rhs)
	};
}

Vector4_s16 Vector4_s16::operator % (s16 rhs) {
	return Vector4_s16 {
		static_cast<s16>(this->x % rhs),
		static_cast<s16>(this->y % rhs),
		static_cast<s16>(this->z % rhs),
		static_cast<s16>(this->w % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector4_u16
//-----------------------------------------------------------------------------

bool Vector4_u16::operator == (Vector4_u16 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y &&
	       this->z == rhs.z &&
	       this->w == rhs.w;
}

bool Vector4_u16::operator == (u16 rhs) {
	return this->x == rhs &&
	       this->y == rhs &&
	       this->z == rhs &&
	       this->w == rhs;
}

u16 & Vector4_u16::operator [] (int index) {
	assert(index >= 0 && index < 4);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
		case 2:  return z;
		case 3:  return w;
	}
}

Vector4_u16 & Vector4_u16::operator += (Vector4_u16 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	this->w += rhs.w;
	return *this;
}

Vector4_u16 & Vector4_u16::operator -= (Vector4_u16 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	this->w -= rhs.w;
	return *this;
}

Vector4_u16 & Vector4_u16::operator *= (Vector4_u16 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	this->w *= rhs.w;
	return *this;
}

Vector4_u16 & Vector4_u16::operator /= (Vector4_u16 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	this->w /= rhs.w;
	return *this;
}

Vector4_u16 & Vector4_u16::operator %= (Vector4_u16 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	this->z %= rhs.z;
	this->w %= rhs.w;
	return *this;
}

Vector4_u16 & Vector4_u16::operator += (u16 rhs) {
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	this->w += rhs;
	return *this;
}

Vector4_u16 & Vector4_u16::operator -= (u16 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	this->w -= rhs;
	return *this;
}

Vector4_u16 & Vector4_u16::operator *= (u16 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	this->w *= rhs;
	return *this;
}

Vector4_u16 & Vector4_u16::operator /= (u16 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	this->w /= rhs;
	return *this;
}

Vector4_u16 & Vector4_u16::operator %= (u16 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	this->z %= rhs;
	this->w %= rhs;
	return *this;
}

Vector4_u16 Vector4_u16::operator + (Vector4_u16 rhs) {
	return Vector4_u16 {
		static_cast<u16>(this->x + rhs.x),
		static_cast<u16>(this->y + rhs.y),
		static_cast<u16>(this->z + rhs.z),
		static_cast<u16>(this->w + rhs.w)
	};
}

Vector4_u16 Vector4_u16::operator - (Vector4_u16 rhs) {
	return Vector4_u16 {
		static_cast<u16>(this->x - rhs.x),
		static_cast<u16>(this->y - rhs.y),
		static_cast<u16>(this->z - rhs.z),
		static_cast<u16>(this->w - rhs.w)
	};
}

Vector4_u16 Vector4_u16::operator * (Vector4_u16 rhs) {
	return Vector4_u16 {
		static_cast<u16>(this->x * rhs.x),
		static_cast<u16>(this->y * rhs.y),
		static_cast<u16>(this->z * rhs.z),
		static_cast<u16>(this->w * rhs.w)
	};
}

Vector4_u16 Vector4_u16::operator / (Vector4_u16 rhs) {
	return Vector4_u16 {
		static_cast<u16>(this->x / rhs.x),
		static_cast<u16>(this->y / rhs.y),
		static_cast<u16>(this->z / rhs.z),
		static_cast<u16>(this->w / rhs.w)
	};
}

Vector4_u16 Vector4_u16::operator % (Vector4_u16 rhs) {
	return Vector4_u16 {
		static_cast<u16>(this->x % rhs.x),
		static_cast<u16>(this->y % rhs.y),
		static_cast<u16>(this->z % rhs.z),
		static_cast<u16>(this->w % rhs.w)
	};
}

Vector4_u16 Vector4_u16::operator + (u16 rhs)  {
	return Vector4_u16 {
		static_cast<u16>(this->x + rhs),
		static_cast<u16>(this->y + rhs),
		static_cast<u16>(this->z + rhs),
		static_cast<u16>(this->w + rhs)
	};
}

Vector4_u16 Vector4_u16::operator - (u16 rhs) {
	return Vector4_u16 {
		static_cast<u16>(this->x - rhs),
		static_cast<u16>(this->y - rhs),
		static_cast<u16>(this->z - rhs),
		static_cast<u16>(this->w - rhs)
	};
}

Vector4_u16 Vector4_u16::operator * (u16 rhs) {
	return Vector4_u16 {
		static_cast<u16>(this->x * rhs),
		static_cast<u16>(this->y * rhs),
		static_cast<u16>(this->z * rhs),
		static_cast<u16>(this->w * rhs)
	};
}

Vector4_u16 Vector4_u16::operator / (u16 rhs) {
	return Vector4_u16 {
		static_cast<u16>(this->x / rhs),
		static_cast<u16>(this->y / rhs),
		static_cast<u16>(this->z / rhs),
		static_cast<u16>(this->w / rhs)
	};
}

Vector4_u16 Vector4_u16::operator % (u16 rhs) {
	return Vector4_u16 {
		static_cast<u16>(this->x % rhs),
		static_cast<u16>(this->y % rhs),
		static_cast<u16>(this->z % rhs),
		static_cast<u16>(this->w % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector4_s32
//-----------------------------------------------------------------------------

bool Vector4_s32::operator == (Vector4_s32 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y &&
	       this->z == rhs.z &&
	       this->w == rhs.w;
}

bool Vector4_s32::operator == (s32 rhs) {
	return this->x == rhs &&
	       this->y == rhs &&
	       this->z == rhs &&
	       this->w == rhs;
}

s32 & Vector4_s32::operator [] (int index) {
	assert(index >= 0 && index < 4);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
		case 2:  return z;
		case 3:  return w;
	}
}

Vector4_s32 & Vector4_s32::operator += (Vector4_s32 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	this->w += rhs.w;
	return *this;
}

Vector4_s32 & Vector4_s32::operator -= (Vector4_s32 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	this->w -= rhs.w;
	return *this;
}

Vector4_s32 & Vector4_s32::operator *= (Vector4_s32 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	this->w *= rhs.w;
	return *this;
}

Vector4_s32 & Vector4_s32::operator /= (Vector4_s32 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	this->w /= rhs.w;
	return *this;
}

Vector4_s32 & Vector4_s32::operator %= (Vector4_s32 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	this->z %= rhs.z;
	this->w %= rhs.w;
	return *this;
}

Vector4_s32 & Vector4_s32::operator += (s32 rhs) {
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	this->w += rhs;
	return *this;
}

Vector4_s32 & Vector4_s32::operator -= (s32 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	this->w -= rhs;
	return *this;
}

Vector4_s32 & Vector4_s32::operator *= (s32 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	this->w *= rhs;
	return *this;
}

Vector4_s32 & Vector4_s32::operator /= (s32 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	this->w /= rhs;
	return *this;
}

Vector4_s32 & Vector4_s32::operator %= (s32 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	this->z %= rhs;
	this->w %= rhs;
	return *this;
}

Vector4_s32 Vector4_s32::operator + (Vector4_s32 rhs) {
	return Vector4_s32 {
		static_cast<s32>(this->x + rhs.x),
		static_cast<s32>(this->y + rhs.y),
		static_cast<s32>(this->z + rhs.z),
		static_cast<s32>(this->w + rhs.w)
	};
}

Vector4_s32 Vector4_s32::operator - (Vector4_s32 rhs) {
	return Vector4_s32 {
		static_cast<s32>(this->x - rhs.x),
		static_cast<s32>(this->y - rhs.y),
		static_cast<s32>(this->z - rhs.z),
		static_cast<s32>(this->w - rhs.w)
	};
}

Vector4_s32 Vector4_s32::operator * (Vector4_s32 rhs) {
	return Vector4_s32 {
		static_cast<s32>(this->x * rhs.x),
		static_cast<s32>(this->y * rhs.y),
		static_cast<s32>(this->z * rhs.z),
		static_cast<s32>(this->w * rhs.w)
	};
}

Vector4_s32 Vector4_s32::operator / (Vector4_s32 rhs) {
	return Vector4_s32 {
		static_cast<s32>(this->x / rhs.x),
		static_cast<s32>(this->y / rhs.y),
		static_cast<s32>(this->z / rhs.z),
		static_cast<s32>(this->w / rhs.w)
	};
}

Vector4_s32 Vector4_s32::operator % (Vector4_s32 rhs) {
	return Vector4_s32 {
		static_cast<s32>(this->x % rhs.x),
		static_cast<s32>(this->y % rhs.y),
		static_cast<s32>(this->z % rhs.z),
		static_cast<s32>(this->w % rhs.w)
	};
}

Vector4_s32 Vector4_s32::operator + (s32 rhs)  {
	return Vector4_s32 {
		static_cast<s32>(this->x + rhs),
		static_cast<s32>(this->y + rhs),
		static_cast<s32>(this->z + rhs),
		static_cast<s32>(this->w + rhs)
	};
}

Vector4_s32 Vector4_s32::operator - (s32 rhs) {
	return Vector4_s32 {
		static_cast<s32>(this->x - rhs),
		static_cast<s32>(this->y - rhs),
		static_cast<s32>(this->z - rhs),
		static_cast<s32>(this->w - rhs)
	};
}

Vector4_s32 Vector4_s32::operator * (s32 rhs) {
	return Vector4_s32 {
		static_cast<s32>(this->x * rhs),
		static_cast<s32>(this->y * rhs),
		static_cast<s32>(this->z * rhs),
		static_cast<s32>(this->w * rhs)
	};
}

Vector4_s32 Vector4_s32::operator / (s32 rhs) {
	return Vector4_s32 {
		static_cast<s32>(this->x / rhs),
		static_cast<s32>(this->y / rhs),
		static_cast<s32>(this->z / rhs),
		static_cast<s32>(this->w / rhs)
	};
}

Vector4_s32 Vector4_s32::operator % (s32 rhs) {
	return Vector4_s32 {
		static_cast<s32>(this->x % rhs),
		static_cast<s32>(this->y % rhs),
		static_cast<s32>(this->z % rhs),
		static_cast<s32>(this->w % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector4_u32
//-----------------------------------------------------------------------------

bool Vector4_u32::operator == (Vector4_u32 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y &&
	       this->z == rhs.z &&
	       this->w == rhs.w;
}

bool Vector4_u32::operator == (u32 rhs) {
	return this->x == rhs &&
	       this->y == rhs &&
	       this->z == rhs &&
	       this->w == rhs;
}

u32 & Vector4_u32::operator [] (int index) {
	assert(index >= 0 && index < 4);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
		case 2:  return z;
		case 3:  return w;
	}
}

Vector4_u32 & Vector4_u32::operator += (Vector4_u32 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	this->w += rhs.w;
	return *this;
}

Vector4_u32 & Vector4_u32::operator -= (Vector4_u32 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	this->w -= rhs.w;
	return *this;
}

Vector4_u32 & Vector4_u32::operator *= (Vector4_u32 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	this->w *= rhs.w;
	return *this;
}

Vector4_u32 & Vector4_u32::operator /= (Vector4_u32 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	this->w /= rhs.w;
	return *this;
}

Vector4_u32 & Vector4_u32::operator %= (Vector4_u32 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	this->z %= rhs.z;
	this->w %= rhs.w;
	return *this;
}

Vector4_u32 & Vector4_u32::operator += (u32 rhs) {
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	this->w += rhs;
	return *this;
}

Vector4_u32 & Vector4_u32::operator -= (u32 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	this->w -= rhs;
	return *this;
}

Vector4_u32 & Vector4_u32::operator *= (u32 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	this->w *= rhs;
	return *this;
}

Vector4_u32 & Vector4_u32::operator /= (u32 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	this->w /= rhs;
	return *this;
}

Vector4_u32 & Vector4_u32::operator %= (u32 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	this->z %= rhs;
	this->w %= rhs;
	return *this;
}

Vector4_u32 Vector4_u32::operator + (Vector4_u32 rhs) {
	return Vector4_u32 {
		static_cast<u32>(this->x + rhs.x),
		static_cast<u32>(this->y + rhs.y),
		static_cast<u32>(this->z + rhs.z),
		static_cast<u32>(this->w + rhs.w)
	};
}

Vector4_u32 Vector4_u32::operator - (Vector4_u32 rhs) {
	return Vector4_u32 {
		static_cast<u32>(this->x - rhs.x),
		static_cast<u32>(this->y - rhs.y),
		static_cast<u32>(this->z - rhs.z),
		static_cast<u32>(this->w - rhs.w)
	};
}

Vector4_u32 Vector4_u32::operator * (Vector4_u32 rhs) {
	return Vector4_u32 {
		static_cast<u32>(this->x * rhs.x),
		static_cast<u32>(this->y * rhs.y),
		static_cast<u32>(this->z * rhs.z),
		static_cast<u32>(this->w * rhs.w)
	};
}

Vector4_u32 Vector4_u32::operator / (Vector4_u32 rhs) {
	return Vector4_u32 {
		static_cast<u32>(this->x / rhs.x),
		static_cast<u32>(this->y / rhs.y),
		static_cast<u32>(this->z / rhs.z),
		static_cast<u32>(this->w / rhs.w)
	};
}

Vector4_u32 Vector4_u32::operator % (Vector4_u32 rhs) {
	return Vector4_u32 {
		static_cast<u32>(this->x % rhs.x),
		static_cast<u32>(this->y % rhs.y),
		static_cast<u32>(this->z % rhs.z),
		static_cast<u32>(this->w % rhs.w)
	};
}

Vector4_u32 Vector4_u32::operator + (u32 rhs)  {
	return Vector4_u32 {
		static_cast<u32>(this->x + rhs),
		static_cast<u32>(this->y + rhs),
		static_cast<u32>(this->z + rhs),
		static_cast<u32>(this->w + rhs)
	};
}

Vector4_u32 Vector4_u32::operator - (u32 rhs) {
	return Vector4_u32 {
		static_cast<u32>(this->x - rhs),
		static_cast<u32>(this->y - rhs),
		static_cast<u32>(this->z - rhs),
		static_cast<u32>(this->w - rhs)
	};
}

Vector4_u32 Vector4_u32::operator * (u32 rhs) {
	return Vector4_u32 {
		static_cast<u32>(this->x * rhs),
		static_cast<u32>(this->y * rhs),
		static_cast<u32>(this->z * rhs),
		static_cast<u32>(this->w * rhs)
	};
}

Vector4_u32 Vector4_u32::operator / (u32 rhs) {
	return Vector4_u32 {
		static_cast<u32>(this->x / rhs),
		static_cast<u32>(this->y / rhs),
		static_cast<u32>(this->z / rhs),
		static_cast<u32>(this->w / rhs)
	};
}

Vector4_u32 Vector4_u32::operator % (u32 rhs) {
	return Vector4_u32 {
		static_cast<u32>(this->x % rhs),
		static_cast<u32>(this->y % rhs),
		static_cast<u32>(this->z % rhs),
		static_cast<u32>(this->w % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector4_s64
//-----------------------------------------------------------------------------

bool Vector4_s64::operator == (Vector4_s64 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y &&
	       this->z == rhs.z &&
	       this->w == rhs.w;
}

bool Vector4_s64::operator == (s64 rhs) {
	return this->x == rhs &&
	       this->y == rhs &&
	       this->z == rhs &&
	       this->w == rhs;
}

s64 & Vector4_s64::operator [] (int index) {
	assert(index >= 0 && index < 4);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
		case 2:  return z;
		case 3:  return w;
	}
}

Vector4_s64 & Vector4_s64::operator += (Vector4_s64 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	this->w += rhs.w;
	return *this;
}

Vector4_s64 & Vector4_s64::operator -= (Vector4_s64 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	this->w -= rhs.w;
	return *this;
}

Vector4_s64 & Vector4_s64::operator *= (Vector4_s64 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	this->w *= rhs.w;
	return *this;
}

Vector4_s64 & Vector4_s64::operator /= (Vector4_s64 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	this->w /= rhs.w;
	return *this;
}

Vector4_s64 & Vector4_s64::operator %= (Vector4_s64 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	this->z %= rhs.z;
	this->w %= rhs.w;
	return *this;
}

Vector4_s64 & Vector4_s64::operator += (s64 rhs) {
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	this->w += rhs;
	return *this;
}

Vector4_s64 & Vector4_s64::operator -= (s64 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	this->w -= rhs;
	return *this;
}

Vector4_s64 & Vector4_s64::operator *= (s64 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	this->w *= rhs;
	return *this;
}

Vector4_s64 & Vector4_s64::operator /= (s64 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	this->w /= rhs;
	return *this;
}

Vector4_s64 & Vector4_s64::operator %= (s64 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	this->z %= rhs;
	this->w %= rhs;
	return *this;
}

Vector4_s64 Vector4_s64::operator + (Vector4_s64 rhs) {
	return Vector4_s64 {
		static_cast<s64>(this->x + rhs.x),
		static_cast<s64>(this->y + rhs.y),
		static_cast<s64>(this->z + rhs.z),
		static_cast<s64>(this->w + rhs.w)
	};
}

Vector4_s64 Vector4_s64::operator - (Vector4_s64 rhs) {
	return Vector4_s64 {
		static_cast<s64>(this->x - rhs.x),
		static_cast<s64>(this->y - rhs.y),
		static_cast<s64>(this->z - rhs.z),
		static_cast<s64>(this->w - rhs.w)
	};
}

Vector4_s64 Vector4_s64::operator * (Vector4_s64 rhs) {
	return Vector4_s64 {
		static_cast<s64>(this->x * rhs.x),
		static_cast<s64>(this->y * rhs.y),
		static_cast<s64>(this->z * rhs.z),
		static_cast<s64>(this->w * rhs.w)
	};
}

Vector4_s64 Vector4_s64::operator / (Vector4_s64 rhs) {
	return Vector4_s64 {
		static_cast<s64>(this->x / rhs.x),
		static_cast<s64>(this->y / rhs.y),
		static_cast<s64>(this->z / rhs.z),
		static_cast<s64>(this->w / rhs.w)
	};
}

Vector4_s64 Vector4_s64::operator % (Vector4_s64 rhs) {
	return Vector4_s64 {
		static_cast<s64>(this->x % rhs.x),
		static_cast<s64>(this->y % rhs.y),
		static_cast<s64>(this->z % rhs.z),
		static_cast<s64>(this->w % rhs.w)
	};
}

Vector4_s64 Vector4_s64::operator + (s64 rhs)  {
	return Vector4_s64 {
		static_cast<s64>(this->x + rhs),
		static_cast<s64>(this->y + rhs),
		static_cast<s64>(this->z + rhs),
		static_cast<s64>(this->w + rhs)
	};
}

Vector4_s64 Vector4_s64::operator - (s64 rhs) {
	return Vector4_s64 {
		static_cast<s64>(this->x - rhs),
		static_cast<s64>(this->y - rhs),
		static_cast<s64>(this->z - rhs),
		static_cast<s64>(this->w - rhs)
	};
}

Vector4_s64 Vector4_s64::operator * (s64 rhs) {
	return Vector4_s64 {
		static_cast<s64>(this->x * rhs),
		static_cast<s64>(this->y * rhs),
		static_cast<s64>(this->z * rhs),
		static_cast<s64>(this->w * rhs)
	};
}

Vector4_s64 Vector4_s64::operator / (s64 rhs) {
	return Vector4_s64 {
		static_cast<s64>(this->x / rhs),
		static_cast<s64>(this->y / rhs),
		static_cast<s64>(this->z / rhs),
		static_cast<s64>(this->w / rhs)
	};
}

Vector4_s64 Vector4_s64::operator % (s64 rhs) {
	return Vector4_s64 {
		static_cast<s64>(this->x % rhs),
		static_cast<s64>(this->y % rhs),
		static_cast<s64>(this->z % rhs),
		static_cast<s64>(this->w % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector4_u64
//-----------------------------------------------------------------------------

bool Vector4_u64::operator == (Vector4_u64 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y &&
	       this->z == rhs.z &&
	       this->w == rhs.w;
}

bool Vector4_u64::operator == (u64 rhs) {
	return this->x == rhs &&
	       this->y == rhs &&
	       this->z == rhs &&
	       this->w == rhs;
}

u64 & Vector4_u64::operator [] (int index) {
	assert(index >= 0 && index < 4);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
		case 2:  return z;
		case 3:  return w;
	}
}

Vector4_u64 & Vector4_u64::operator += (Vector4_u64 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	this->w += rhs.w;
	return *this;
}

Vector4_u64 & Vector4_u64::operator -= (Vector4_u64 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	this->w -= rhs.w;
	return *this;
}

Vector4_u64 & Vector4_u64::operator *= (Vector4_u64 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	this->w *= rhs.w;
	return *this;
}

Vector4_u64 & Vector4_u64::operator /= (Vector4_u64 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	this->w /= rhs.w;
	return *this;
}

Vector4_u64 & Vector4_u64::operator %= (Vector4_u64 rhs) {
	this->x %= rhs.x;
	this->y %= rhs.y;
	this->z %= rhs.z;
	this->w %= rhs.w;
	return *this;
}

Vector4_u64 & Vector4_u64::operator += (u64 rhs) {
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	this->w += rhs;
	return *this;
}

Vector4_u64 & Vector4_u64::operator -= (u64 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	this->w -= rhs;
	return *this;
}

Vector4_u64 & Vector4_u64::operator *= (u64 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	this->w *= rhs;
	return *this;
}

Vector4_u64 & Vector4_u64::operator /= (u64 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	this->w /= rhs;
	return *this;
}

Vector4_u64 & Vector4_u64::operator %= (u64 rhs) {
	this->x %= rhs;
	this->y %= rhs;
	this->z %= rhs;
	this->w %= rhs;
	return *this;
}

Vector4_u64 Vector4_u64::operator + (Vector4_u64 rhs) {
	return Vector4_u64 {
		static_cast<u64>(this->x + rhs.x),
		static_cast<u64>(this->y + rhs.y),
		static_cast<u64>(this->z + rhs.z),
		static_cast<u64>(this->w + rhs.w)
	};
}

Vector4_u64 Vector4_u64::operator - (Vector4_u64 rhs) {
	return Vector4_u64 {
		static_cast<u64>(this->x - rhs.x),
		static_cast<u64>(this->y - rhs.y),
		static_cast<u64>(this->z - rhs.z),
		static_cast<u64>(this->w - rhs.w)
	};
}

Vector4_u64 Vector4_u64::operator * (Vector4_u64 rhs) {
	return Vector4_u64 {
		static_cast<u64>(this->x * rhs.x),
		static_cast<u64>(this->y * rhs.y),
		static_cast<u64>(this->z * rhs.z),
		static_cast<u64>(this->w * rhs.w)
	};
}

Vector4_u64 Vector4_u64::operator / (Vector4_u64 rhs) {
	return Vector4_u64 {
		static_cast<u64>(this->x / rhs.x),
		static_cast<u64>(this->y / rhs.y),
		static_cast<u64>(this->z / rhs.z),
		static_cast<u64>(this->w / rhs.w)
	};
}

Vector4_u64 Vector4_u64::operator % (Vector4_u64 rhs) {
	return Vector4_u64 {
		static_cast<u64>(this->x % rhs.x),
		static_cast<u64>(this->y % rhs.y),
		static_cast<u64>(this->z % rhs.z),
		static_cast<u64>(this->w % rhs.w)
	};
}

Vector4_u64 Vector4_u64::operator + (u64 rhs)  {
	return Vector4_u64 {
		static_cast<u64>(this->x + rhs),
		static_cast<u64>(this->y + rhs),
		static_cast<u64>(this->z + rhs),
		static_cast<u64>(this->w + rhs)
	};
}

Vector4_u64 Vector4_u64::operator - (u64 rhs) {
	return Vector4_u64 {
		static_cast<u64>(this->x - rhs),
		static_cast<u64>(this->y - rhs),
		static_cast<u64>(this->z - rhs),
		static_cast<u64>(this->w - rhs)
	};
}

Vector4_u64 Vector4_u64::operator * (u64 rhs) {
	return Vector4_u64 {
		static_cast<u64>(this->x * rhs),
		static_cast<u64>(this->y * rhs),
		static_cast<u64>(this->z * rhs),
		static_cast<u64>(this->w * rhs)
	};
}

Vector4_u64 Vector4_u64::operator / (u64 rhs) {
	return Vector4_u64 {
		static_cast<u64>(this->x / rhs),
		static_cast<u64>(this->y / rhs),
		static_cast<u64>(this->z / rhs),
		static_cast<u64>(this->w / rhs)
	};
}

Vector4_u64 Vector4_u64::operator % (u64 rhs) {
	return Vector4_u64 {
		static_cast<u64>(this->x % rhs),
		static_cast<u64>(this->y % rhs),
		static_cast<u64>(this->z % rhs),
		static_cast<u64>(this->w % rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector4_f32
//-----------------------------------------------------------------------------

bool Vector4_f32::operator == (Vector4_f32 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y &&
	       this->z == rhs.z &&
	       this->w == rhs.w;
}

bool Vector4_f32::operator == (f32 rhs) {
	return this->x == rhs &&
	       this->y == rhs &&
	       this->z == rhs &&
	       this->w == rhs;
}

f32 & Vector4_f32::operator [] (int index) {
	assert(index >= 0 && index < 4);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
		case 2:  return z;
		case 3:  return w;
	}
}

Vector4_f32 & Vector4_f32::operator += (Vector4_f32 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	this->w += rhs.w;
	return *this;
}

Vector4_f32 & Vector4_f32::operator -= (Vector4_f32 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	this->w -= rhs.w;
	return *this;
}

Vector4_f32 & Vector4_f32::operator *= (Vector4_f32 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	this->w *= rhs.w;
	return *this;
}

Vector4_f32 & Vector4_f32::operator /= (Vector4_f32 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	this->w /= rhs.w;
	return *this;
}

Vector4_f32 & Vector4_f32::operator += (f32 rhs) {
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	this->w += rhs;
	return *this;
}

Vector4_f32 & Vector4_f32::operator -= (f32 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	this->w -= rhs;
	return *this;
}

Vector4_f32 & Vector4_f32::operator *= (f32 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	this->w *= rhs;
	return *this;
}

Vector4_f32 & Vector4_f32::operator /= (f32 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	this->w /= rhs;
	return *this;
}

Vector4_f32 Vector4_f32::operator + (Vector4_f32 rhs) {
	return Vector4_f32 {
		static_cast<f32>(this->x + rhs.x),
		static_cast<f32>(this->y + rhs.y),
		static_cast<f32>(this->z + rhs.z),
		static_cast<f32>(this->w + rhs.w)
	};
}

Vector4_f32 Vector4_f32::operator - (Vector4_f32 rhs) {
	return Vector4_f32 {
		static_cast<f32>(this->x - rhs.x),
		static_cast<f32>(this->y - rhs.y),
		static_cast<f32>(this->z - rhs.z),
		static_cast<f32>(this->w - rhs.w)
	};
}

Vector4_f32 Vector4_f32::operator * (Vector4_f32 rhs) {
	return Vector4_f32 {
		static_cast<f32>(this->x * rhs.x),
		static_cast<f32>(this->y * rhs.y),
		static_cast<f32>(this->z * rhs.z),
		static_cast<f32>(this->w * rhs.w)
	};
}

Vector4_f32 Vector4_f32::operator / (Vector4_f32 rhs) {
	return Vector4_f32 {
		static_cast<f32>(this->x / rhs.x),
		static_cast<f32>(this->y / rhs.y),
		static_cast<f32>(this->z / rhs.z),
		static_cast<f32>(this->w / rhs.w)
	};
}

Vector4_f32 Vector4_f32::operator + (f32 rhs)  {
	return Vector4_f32 {
		static_cast<f32>(this->x + rhs),
		static_cast<f32>(this->y + rhs),
		static_cast<f32>(this->z + rhs),
		static_cast<f32>(this->w + rhs)
	};
}

Vector4_f32 Vector4_f32::operator - (f32 rhs) {
	return Vector4_f32 {
		static_cast<f32>(this->x - rhs),
		static_cast<f32>(this->y - rhs),
		static_cast<f32>(this->z - rhs),
		static_cast<f32>(this->w - rhs)
	};
}

Vector4_f32 Vector4_f32::operator * (f32 rhs) {
	return Vector4_f32 {
		static_cast<f32>(this->x * rhs),
		static_cast<f32>(this->y * rhs),
		static_cast<f32>(this->z * rhs),
		static_cast<f32>(this->w * rhs)
	};
}

Vector4_f32 Vector4_f32::operator / (f32 rhs) {
	return Vector4_f32 {
		static_cast<f32>(this->x / rhs),
		static_cast<f32>(this->y / rhs),
		static_cast<f32>(this->z / rhs),
		static_cast<f32>(this->w / rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Vector4_f64
//-----------------------------------------------------------------------------

bool Vector4_f64::operator == (Vector4_f64 rhs) {
	return this->x == rhs.x &&
	       this->y == rhs.y &&
	       this->z == rhs.z &&
	       this->w == rhs.w;
}

bool Vector4_f64::operator == (f64 rhs) {
	return this->x == rhs &&
	       this->y == rhs &&
	       this->z == rhs &&
	       this->w == rhs;
}

f64 & Vector4_f64::operator [] (int index) {
	assert(index >= 0 && index < 4);
	switch (index) {
		default:
		case 0:  return x;
		case 1:  return y;
		case 2:  return z;
		case 3:  return w;
	}
}

Vector4_f64 & Vector4_f64::operator += (Vector4_f64 rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	this->w += rhs.w;
	return *this;
}

Vector4_f64 & Vector4_f64::operator -= (Vector4_f64 rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	this->w -= rhs.w;
	return *this;
}

Vector4_f64 & Vector4_f64::operator *= (Vector4_f64 rhs) {
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	this->w *= rhs.w;
	return *this;
}

Vector4_f64 & Vector4_f64::operator /= (Vector4_f64 rhs) {
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	this->w /= rhs.w;
	return *this;
}

Vector4_f64 & Vector4_f64::operator += (f64 rhs) {
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	this->w += rhs;
	return *this;
}

Vector4_f64 & Vector4_f64::operator -= (f64 rhs) {
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	this->w -= rhs;
	return *this;
}

Vector4_f64 & Vector4_f64::operator *= (f64 rhs) {
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	this->w *= rhs;
	return *this;
}

Vector4_f64 & Vector4_f64::operator /= (f64 rhs) {
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	this->w /= rhs;
	return *this;
}

Vector4_f64 Vector4_f64::operator + (Vector4_f64 rhs) {
	return Vector4_f64 {
		static_cast<f64>(this->x + rhs.x),
		static_cast<f64>(this->y + rhs.y),
		static_cast<f64>(this->z + rhs.z),
		static_cast<f64>(this->w + rhs.w)
	};
}

Vector4_f64 Vector4_f64::operator - (Vector4_f64 rhs) {
	return Vector4_f64 {
		static_cast<f64>(this->x - rhs.x),
		static_cast<f64>(this->y - rhs.y),
		static_cast<f64>(this->z - rhs.z),
		static_cast<f64>(this->w - rhs.w)
	};
}

Vector4_f64 Vector4_f64::operator * (Vector4_f64 rhs) {
	return Vector4_f64 {
		static_cast<f64>(this->x * rhs.x),
		static_cast<f64>(this->y * rhs.y),
		static_cast<f64>(this->z * rhs.z),
		static_cast<f64>(this->w * rhs.w)
	};
}

Vector4_f64 Vector4_f64::operator / (Vector4_f64 rhs) {
	return Vector4_f64 {
		static_cast<f64>(this->x / rhs.x),
		static_cast<f64>(this->y / rhs.y),
		static_cast<f64>(this->z / rhs.z),
		static_cast<f64>(this->w / rhs.w)
	};
}

Vector4_f64 Vector4_f64::operator + (f64 rhs)  {
	return Vector4_f64 {
		static_cast<f64>(this->x + rhs),
		static_cast<f64>(this->y + rhs),
		static_cast<f64>(this->z + rhs),
		static_cast<f64>(this->w + rhs)
	};
}

Vector4_f64 Vector4_f64::operator - (f64 rhs) {
	return Vector4_f64 {
		static_cast<f64>(this->x - rhs),
		static_cast<f64>(this->y - rhs),
		static_cast<f64>(this->z - rhs),
		static_cast<f64>(this->w - rhs)
	};
}

Vector4_f64 Vector4_f64::operator * (f64 rhs) {
	return Vector4_f64 {
		static_cast<f64>(this->x * rhs),
		static_cast<f64>(this->y * rhs),
		static_cast<f64>(this->z * rhs),
		static_cast<f64>(this->w * rhs)
	};
}

Vector4_f64 Vector4_f64::operator / (f64 rhs) {
	return Vector4_f64 {
		static_cast<f64>(this->x / rhs),
		static_cast<f64>(this->y / rhs),
		static_cast<f64>(this->z / rhs),
		static_cast<f64>(this->w / rhs)
	};
}

//-----------------------------------------------------------------------------
// [SECTION] Matrix3x3_f32 matrix functions
//-----------------------------------------------------------------------------

Matrix3x3_f32 matrix3x3_f32_transpose(Matrix3x3_f32 matrix) {
	Matrix3x3_f32 transpose;

	transpose[0][0] = matrix[0][0];
	transpose[0][1] = matrix[1][0];
	transpose[0][2] = matrix[2][0];

	transpose[1][0] = matrix[0][1];
	transpose[1][1] = matrix[1][1];
	transpose[1][2] = matrix[2][1];

	transpose[2][0] = matrix[0][2];
	transpose[2][1] = matrix[1][2];
	transpose[2][2] = matrix[2][2];

	return transpose;
}

f32 matrix3x3_f32_determinant(Matrix3x3_f32 matrix) {
	return  + matrix[0][0] * ( matrix[1][1] * matrix[2][2]  -  matrix[2][1] * matrix[1][2] )
	        - matrix[1][0] * ( matrix[0][1] * matrix[2][2]  -  matrix[2][1] * matrix[0][2] )
	        + matrix[2][0] * ( matrix[0][1] * matrix[1][2]  -  matrix[1][1] * matrix[0][2] );
}

Matrix3x3_f32 matrix3x3_f32_inverse(Matrix3x3_f32 matrix) {
	f32 one_over_determinant = 1.0f / (
		+ matrix[0][0] * ( matrix[1][1] * matrix[2][2]  -  matrix[2][1] * matrix[1][2] )
		- matrix[1][0] * ( matrix[0][1] * matrix[2][2]  -  matrix[2][1] * matrix[0][2] )
		+ matrix[2][0] * ( matrix[0][1] * matrix[1][2]  -  matrix[1][1] * matrix[0][2] )
	);

	Matrix3x3_f32 inverse;
	inverse[0][0] = + ( matrix[1][1] * matrix[2][2]  -  matrix[2][1] * matrix[1][2] ) * one_over_determinant;
	inverse[1][0] = - ( matrix[1][0] * matrix[2][2]  -  matrix[2][0] * matrix[1][2] ) * one_over_determinant;
	inverse[2][0] = + ( matrix[1][0] * matrix[2][1]  -  matrix[2][0] * matrix[1][1] ) * one_over_determinant;
	inverse[0][1] = - ( matrix[0][1] * matrix[2][2]  -  matrix[2][1] * matrix[0][2] ) * one_over_determinant;
	inverse[1][1] = + ( matrix[0][0] * matrix[2][2]  -  matrix[2][0] * matrix[0][2] ) * one_over_determinant;
	inverse[2][1] = - ( matrix[0][0] * matrix[2][1]  -  matrix[2][0] * matrix[0][1] ) * one_over_determinant;
	inverse[0][2] = + ( matrix[0][1] * matrix[1][2]  -  matrix[1][1] * matrix[0][2] ) * one_over_determinant;
	inverse[1][2] = - ( matrix[0][0] * matrix[1][2]  -  matrix[1][0] * matrix[0][2] ) * one_over_determinant;
	inverse[2][2] = + ( matrix[0][0] * matrix[1][1]  -  matrix[1][0] * matrix[0][1] ) * one_over_determinant;

	return inverse;
}

Matrix3x3_f32 matrix3x3_f32_multiply(Matrix3x3_f32 lhs, Matrix3x3_f32 rhs) {
	Matrix3x3_f32 result;
	result[0][0] =  lhs[0][0] * rhs[0][0]  +  lhs[1][0] * rhs[0][1]  +  lhs[2][0] * rhs[0][2];
	result[0][1] =  lhs[0][1] * rhs[0][0]  +  lhs[1][1] * rhs[0][1]  +  lhs[2][1] * rhs[0][2];
	result[0][2] =  lhs[0][2] * rhs[0][0]  +  lhs[1][2] * rhs[0][1]  +  lhs[2][2] * rhs[0][2];
	result[1][0] =  lhs[0][0] * rhs[1][0]  +  lhs[1][0] * rhs[1][1]  +  lhs[2][0] * rhs[1][2];
	result[1][1] =  lhs[0][1] * rhs[1][0]  +  lhs[1][1] * rhs[1][1]  +  lhs[2][1] * rhs[1][2];
	result[1][2] =  lhs[0][2] * rhs[1][0]  +  lhs[1][2] * rhs[1][1]  +  lhs[2][2] * rhs[1][2];
	result[2][0] =  lhs[0][0] * rhs[2][0]  +  lhs[1][0] * rhs[2][1]  +  lhs[2][0] * rhs[2][2];
	result[2][1] =  lhs[0][1] * rhs[2][0]  +  lhs[1][1] * rhs[2][1]  +  lhs[2][1] * rhs[2][2];
	result[2][2] =  lhs[0][2] * rhs[2][0]  +  lhs[1][2] * rhs[2][1]  +  lhs[2][2] * rhs[2][2];
	return result;
}


//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Matrix3x3_f32
//-----------------------------------------------------------------------------

Matrix3x3_f32::Matrix3x3_f32() {
	this->columns[0] = Vector3_f32 { 1, 0, 0 };
	this->columns[1] = Vector3_f32 { 0, 1, 0 };
	this->columns[2] = Vector3_f32 { 0, 0, 1 };
}

Matrix3x3_f32::Matrix3x3_f32(f32 value) {
	this->columns[0] = Vector3_f32 { value,   0,     0   };
	this->columns[1] = Vector3_f32 {   0,   value,   0   };
	this->columns[2] = Vector3_f32 {   0,     0,   value };
}

Matrix3x3_f32::Matrix3x3_f32(Vector3_f32 column0, Vector3_f32 column1, Vector3_f32 column2) {
	this->columns[0] = column0;
	this->columns[1] = column1;
	this->columns[2] = column2;
}

bool Matrix3x3_f32::operator == (Matrix3x3_f32 rhs) {
	return this->columns[0] == rhs[0] &&
	       this->columns[1] == rhs[1] &&
	       this->columns[2] == rhs[2];
}

bool Matrix3x3_f32::operator == (f32 rhs) {
	return this->columns[0] == rhs &&
	       this->columns[1] == rhs &&
	       this->columns[2] == rhs;
}

Vector3_f32 & Matrix3x3_f32::operator [] (int index) {
	assert(index >= 0 && index < 3);
	return this->columns[index];
}

Matrix3x3_f32 & Matrix3x3_f32::operator += (Matrix3x3_f32 rhs) {
	this->columns[0] += rhs[0];
	this->columns[1] += rhs[1];
	this->columns[2] += rhs[2];
	return *this;
}

Matrix3x3_f32 & Matrix3x3_f32::operator -= (Matrix3x3_f32 rhs) {
	this->columns[0] -= rhs[0];
	this->columns[1] -= rhs[1];
	this->columns[2] -= rhs[2];
	return *this;
}

Matrix3x3_f32 & Matrix3x3_f32::operator *= (Matrix3x3_f32 rhs) {
	*this = *this * rhs;
	return *this;
}

Matrix3x3_f32 & Matrix3x3_f32::operator /= (Matrix3x3_f32 rhs) {
	*this *= matrix3x3_f32_inverse(rhs);
	return *this;
}

Matrix3x3_f32 & Matrix3x3_f32::operator += (f32 rhs) {
	this->columns[0] += rhs;
	this->columns[1] += rhs;
	this->columns[2] += rhs;
	return *this;
}

Matrix3x3_f32 & Matrix3x3_f32::operator -= (f32 rhs) {
	this->columns[0] -= rhs;
	this->columns[1] -= rhs;
	this->columns[2] -= rhs;
	return *this;
}

Matrix3x3_f32 & Matrix3x3_f32::operator *= (f32 rhs) {
	this->columns[0] *= rhs;
	this->columns[1] *= rhs;
	this->columns[2] *= rhs;
	return *this;
}

Matrix3x3_f32 & Matrix3x3_f32::operator /= (f32 rhs) {
	this->columns[0] /= rhs;
	this->columns[1] /= rhs;
	this->columns[2] /= rhs;
	return *this;
}

Matrix3x3_f32 Matrix3x3_f32::operator + (Matrix3x3_f32 rhs) {
	return Matrix3x3_f32(
		this->columns[0] + rhs[0],
		this->columns[1] + rhs[1],
		this->columns[2] + rhs[2]
	);
}

Matrix3x3_f32 Matrix3x3_f32::operator - (Matrix3x3_f32 rhs) {
	return Matrix3x3_f32(
		this->columns[0] - rhs[0],
		this->columns[1] - rhs[1],
		this->columns[2] - rhs[2]
	);
}

Matrix3x3_f32 Matrix3x3_f32::operator * (Matrix3x3_f32 rhs) {
	Matrix3x3_f32 result;
	result[0][0] =  this->columns[0][0] * rhs[0][0]  +  this->columns[1][0] * rhs[0][1]  +  this->columns[2][0] * rhs[0][2];
	result[0][1] =  this->columns[0][1] * rhs[0][0]  +  this->columns[1][1] * rhs[0][1]  +  this->columns[2][1] * rhs[0][2];
	result[0][2] =  this->columns[0][2] * rhs[0][0]  +  this->columns[1][2] * rhs[0][1]  +  this->columns[2][2] * rhs[0][2];
	result[1][0] =  this->columns[0][0] * rhs[1][0]  +  this->columns[1][0] * rhs[1][1]  +  this->columns[2][0] * rhs[1][2];
	result[1][1] =  this->columns[0][1] * rhs[1][0]  +  this->columns[1][1] * rhs[1][1]  +  this->columns[2][1] * rhs[1][2];
	result[1][2] =  this->columns[0][2] * rhs[1][0]  +  this->columns[1][2] * rhs[1][1]  +  this->columns[2][2] * rhs[1][2];
	result[2][0] =  this->columns[0][0] * rhs[2][0]  +  this->columns[1][0] * rhs[2][1]  +  this->columns[2][0] * rhs[2][2];
	result[2][1] =  this->columns[0][1] * rhs[2][0]  +  this->columns[1][1] * rhs[2][1]  +  this->columns[2][1] * rhs[2][2];
	result[2][2] =  this->columns[0][2] * rhs[2][0]  +  this->columns[1][2] * rhs[2][1]  +  this->columns[2][2] * rhs[2][2];
	return result;
}

Matrix3x3_f32 Matrix3x3_f32::operator / (Matrix3x3_f32 rhs) {
	Matrix3x3_f32 result = { *this };
	result /= rhs;
	return result;
}

Matrix3x3_f32 Matrix3x3_f32::operator + (f32 rhs)  {
	return Matrix3x3_f32(
		this->columns[0] + rhs,
		this->columns[1] + rhs,
		this->columns[2] + rhs
	);
}

Matrix3x3_f32 Matrix3x3_f32::operator - (f32 rhs) {
	return Matrix3x3_f32(
		this->columns[0] - rhs,
		this->columns[1] - rhs,
		this->columns[2] - rhs
	);
}

Matrix3x3_f32 Matrix3x3_f32::operator * (f32 rhs) {
	return Matrix3x3_f32(
		this->columns[0] * rhs,
		this->columns[1] * rhs,
		this->columns[2] * rhs
	);
}

Matrix3x3_f32 Matrix3x3_f32::operator / (f32 rhs) {
	return Matrix3x3_f32(
		this->columns[0] / rhs,
		this->columns[1] / rhs,
		this->columns[2] / rhs
	);
}

//-----------------------------------------------------------------------------
// [SECTION] Matrix3x3_f64 matrix functions
//-----------------------------------------------------------------------------

Matrix3x3_f64 matrix3x3_f64_transpose(Matrix3x3_f64 matrix) {
	Matrix3x3_f64 transpose;

	transpose[0][0] = matrix[0][0];
	transpose[0][1] = matrix[1][0];
	transpose[0][2] = matrix[2][0];

	transpose[1][0] = matrix[0][1];
	transpose[1][1] = matrix[1][1];
	transpose[1][2] = matrix[2][1];

	transpose[2][0] = matrix[0][2];
	transpose[2][1] = matrix[1][2];
	transpose[2][2] = matrix[2][2];

	return transpose;
}

f64 matrix3x3_f64_determinant(Matrix3x3_f64 matrix) {
	return  + matrix[0][0] * ( matrix[1][1] * matrix[2][2]  -  matrix[2][1] * matrix[1][2] )
	        - matrix[1][0] * ( matrix[0][1] * matrix[2][2]  -  matrix[2][1] * matrix[0][2] )
	        + matrix[2][0] * ( matrix[0][1] * matrix[1][2]  -  matrix[1][1] * matrix[0][2] );
}

Matrix3x3_f64 matrix3x3_f64_inverse(Matrix3x3_f64 matrix) {
	f64 one_over_determinant = 1.0f / (
		+ matrix[0][0] * ( matrix[1][1] * matrix[2][2]  -  matrix[2][1] * matrix[1][2] )
		- matrix[1][0] * ( matrix[0][1] * matrix[2][2]  -  matrix[2][1] * matrix[0][2] )
		+ matrix[2][0] * ( matrix[0][1] * matrix[1][2]  -  matrix[1][1] * matrix[0][2] )
	);

	Matrix3x3_f64 inverse;
	inverse[0][0] = + ( matrix[1][1] * matrix[2][2]  -  matrix[2][1] * matrix[1][2] ) * one_over_determinant;
	inverse[1][0] = - ( matrix[1][0] * matrix[2][2]  -  matrix[2][0] * matrix[1][2] ) * one_over_determinant;
	inverse[2][0] = + ( matrix[1][0] * matrix[2][1]  -  matrix[2][0] * matrix[1][1] ) * one_over_determinant;
	inverse[0][1] = - ( matrix[0][1] * matrix[2][2]  -  matrix[2][1] * matrix[0][2] ) * one_over_determinant;
	inverse[1][1] = + ( matrix[0][0] * matrix[2][2]  -  matrix[2][0] * matrix[0][2] ) * one_over_determinant;
	inverse[2][1] = - ( matrix[0][0] * matrix[2][1]  -  matrix[2][0] * matrix[0][1] ) * one_over_determinant;
	inverse[0][2] = + ( matrix[0][1] * matrix[1][2]  -  matrix[1][1] * matrix[0][2] ) * one_over_determinant;
	inverse[1][2] = - ( matrix[0][0] * matrix[1][2]  -  matrix[1][0] * matrix[0][2] ) * one_over_determinant;
	inverse[2][2] = + ( matrix[0][0] * matrix[1][1]  -  matrix[1][0] * matrix[0][1] ) * one_over_determinant;

	return inverse;
}

Matrix3x3_f64 matrix3x3_f64_multiply(Matrix3x3_f64 lhs, Matrix3x3_f64 rhs) {
	Matrix3x3_f64 result;
	result[0][0] =  lhs[0][0] * rhs[0][0]  +  lhs[1][0] * rhs[0][1]  +  lhs[2][0] * rhs[0][2];
	result[0][1] =  lhs[0][1] * rhs[0][0]  +  lhs[1][1] * rhs[0][1]  +  lhs[2][1] * rhs[0][2];
	result[0][2] =  lhs[0][2] * rhs[0][0]  +  lhs[1][2] * rhs[0][1]  +  lhs[2][2] * rhs[0][2];
	result[1][0] =  lhs[0][0] * rhs[1][0]  +  lhs[1][0] * rhs[1][1]  +  lhs[2][0] * rhs[1][2];
	result[1][1] =  lhs[0][1] * rhs[1][0]  +  lhs[1][1] * rhs[1][1]  +  lhs[2][1] * rhs[1][2];
	result[1][2] =  lhs[0][2] * rhs[1][0]  +  lhs[1][2] * rhs[1][1]  +  lhs[2][2] * rhs[1][2];
	result[2][0] =  lhs[0][0] * rhs[2][0]  +  lhs[1][0] * rhs[2][1]  +  lhs[2][0] * rhs[2][2];
	result[2][1] =  lhs[0][1] * rhs[2][0]  +  lhs[1][1] * rhs[2][1]  +  lhs[2][1] * rhs[2][2];
	result[2][2] =  lhs[0][2] * rhs[2][0]  +  lhs[1][2] * rhs[2][1]  +  lhs[2][2] * rhs[2][2];
	return result;
}


//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Matrix3x3_f64
//-----------------------------------------------------------------------------

Matrix3x3_f64::Matrix3x3_f64() {
	this->columns[0] = Vector3_f64 { 1, 0, 0 };
	this->columns[1] = Vector3_f64 { 0, 1, 0 };
	this->columns[2] = Vector3_f64 { 0, 0, 1 };
}

Matrix3x3_f64::Matrix3x3_f64(f64 value) {
	this->columns[0] = Vector3_f64 { value,   0,     0   };
	this->columns[1] = Vector3_f64 {   0,   value,   0   };
	this->columns[2] = Vector3_f64 {   0,     0,   value };
}

Matrix3x3_f64::Matrix3x3_f64(Vector3_f64 column0, Vector3_f64 column1, Vector3_f64 column2) {
	this->columns[0] = column0;
	this->columns[1] = column1;
	this->columns[2] = column2;
}

bool Matrix3x3_f64::operator == (Matrix3x3_f64 rhs) {
	return this->columns[0] == rhs[0] &&
	       this->columns[1] == rhs[1] &&
	       this->columns[2] == rhs[2];
}

bool Matrix3x3_f64::operator == (f64 rhs) {
	return this->columns[0] == rhs &&
	       this->columns[1] == rhs &&
	       this->columns[2] == rhs;
}

Vector3_f64 & Matrix3x3_f64::operator [] (int index) {
	assert(index >= 0 && index < 3);
	return this->columns[index];
}

Matrix3x3_f64 & Matrix3x3_f64::operator += (Matrix3x3_f64 rhs) {
	this->columns[0] += rhs[0];
	this->columns[1] += rhs[1];
	this->columns[2] += rhs[2];
	return *this;
}

Matrix3x3_f64 & Matrix3x3_f64::operator -= (Matrix3x3_f64 rhs) {
	this->columns[0] -= rhs[0];
	this->columns[1] -= rhs[1];
	this->columns[2] -= rhs[2];
	return *this;
}

Matrix3x3_f64 & Matrix3x3_f64::operator *= (Matrix3x3_f64 rhs) {
	*this = *this * rhs;
	return *this;
}

Matrix3x3_f64 & Matrix3x3_f64::operator /= (Matrix3x3_f64 rhs) {
	*this *= matrix3x3_f64_inverse(rhs);
	return *this;
}

Matrix3x3_f64 & Matrix3x3_f64::operator += (f64 rhs) {
	this->columns[0] += rhs;
	this->columns[1] += rhs;
	this->columns[2] += rhs;
	return *this;
}

Matrix3x3_f64 & Matrix3x3_f64::operator -= (f64 rhs) {
	this->columns[0] -= rhs;
	this->columns[1] -= rhs;
	this->columns[2] -= rhs;
	return *this;
}

Matrix3x3_f64 & Matrix3x3_f64::operator *= (f64 rhs) {
	this->columns[0] *= rhs;
	this->columns[1] *= rhs;
	this->columns[2] *= rhs;
	return *this;
}

Matrix3x3_f64 & Matrix3x3_f64::operator /= (f64 rhs) {
	this->columns[0] /= rhs;
	this->columns[1] /= rhs;
	this->columns[2] /= rhs;
	return *this;
}

Matrix3x3_f64 Matrix3x3_f64::operator + (Matrix3x3_f64 rhs) {
	return Matrix3x3_f64(
		this->columns[0] + rhs[0],
		this->columns[1] + rhs[1],
		this->columns[2] + rhs[2]
	);
}

Matrix3x3_f64 Matrix3x3_f64::operator - (Matrix3x3_f64 rhs) {
	return Matrix3x3_f64(
		this->columns[0] - rhs[0],
		this->columns[1] - rhs[1],
		this->columns[2] - rhs[2]
	);
}

Matrix3x3_f64 Matrix3x3_f64::operator * (Matrix3x3_f64 rhs) {
	Matrix3x3_f64 result;
	result[0][0] =  this->columns[0][0] * rhs[0][0]  +  this->columns[1][0] * rhs[0][1]  +  this->columns[2][0] * rhs[0][2];
	result[0][1] =  this->columns[0][1] * rhs[0][0]  +  this->columns[1][1] * rhs[0][1]  +  this->columns[2][1] * rhs[0][2];
	result[0][2] =  this->columns[0][2] * rhs[0][0]  +  this->columns[1][2] * rhs[0][1]  +  this->columns[2][2] * rhs[0][2];
	result[1][0] =  this->columns[0][0] * rhs[1][0]  +  this->columns[1][0] * rhs[1][1]  +  this->columns[2][0] * rhs[1][2];
	result[1][1] =  this->columns[0][1] * rhs[1][0]  +  this->columns[1][1] * rhs[1][1]  +  this->columns[2][1] * rhs[1][2];
	result[1][2] =  this->columns[0][2] * rhs[1][0]  +  this->columns[1][2] * rhs[1][1]  +  this->columns[2][2] * rhs[1][2];
	result[2][0] =  this->columns[0][0] * rhs[2][0]  +  this->columns[1][0] * rhs[2][1]  +  this->columns[2][0] * rhs[2][2];
	result[2][1] =  this->columns[0][1] * rhs[2][0]  +  this->columns[1][1] * rhs[2][1]  +  this->columns[2][1] * rhs[2][2];
	result[2][2] =  this->columns[0][2] * rhs[2][0]  +  this->columns[1][2] * rhs[2][1]  +  this->columns[2][2] * rhs[2][2];
	return result;
}

Matrix3x3_f64 Matrix3x3_f64::operator / (Matrix3x3_f64 rhs) {
	Matrix3x3_f64 result = { *this };
	result /= rhs;
	return result;
}

Matrix3x3_f64 Matrix3x3_f64::operator + (f64 rhs)  {
	return Matrix3x3_f64(
		this->columns[0] + rhs,
		this->columns[1] + rhs,
		this->columns[2] + rhs
	);
}

Matrix3x3_f64 Matrix3x3_f64::operator - (f64 rhs) {
	return Matrix3x3_f64(
		this->columns[0] - rhs,
		this->columns[1] - rhs,
		this->columns[2] - rhs
	);
}

Matrix3x3_f64 Matrix3x3_f64::operator * (f64 rhs) {
	return Matrix3x3_f64(
		this->columns[0] * rhs,
		this->columns[1] * rhs,
		this->columns[2] * rhs
	);
}

Matrix3x3_f64 Matrix3x3_f64::operator / (f64 rhs) {
	return Matrix3x3_f64(
		this->columns[0] / rhs,
		this->columns[1] / rhs,
		this->columns[2] / rhs
	);
}

//-----------------------------------------------------------------------------
// [SECTION] Matrix4x4_f32 matrix functions
//-----------------------------------------------------------------------------

Matrix4x4_f32 matrix4x4_f32_transpose(Matrix4x4_f32 matrix) {
	Matrix4x4_f32 transpose;

	transpose[0][0] = matrix[0][0];
	transpose[0][1] = matrix[1][0];
	transpose[0][2] = matrix[2][0];
	transpose[0][3] = matrix[3][0];

	transpose[1][0] = matrix[0][1];
	transpose[1][1] = matrix[1][1];
	transpose[1][2] = matrix[2][1];
	transpose[1][3] = matrix[3][1];

	transpose[2][0] = matrix[0][2];
	transpose[2][1] = matrix[1][2];
	transpose[2][2] = matrix[2][2];
	transpose[2][3] = matrix[3][2];

	transpose[3][0] = matrix[0][3];
	transpose[3][1] = matrix[1][3];
	transpose[3][2] = matrix[2][3];
	transpose[3][3] = matrix[3][3];

	return transpose;
}

f32 matrix4x4_f32_determinant(Matrix4x4_f32 matrix) {
	f32 sub_factor_00 =  matrix[2][2] * matrix[3][3]  -  matrix[3][2] * matrix[2][3];
	f32 sub_factor_01 =  matrix[2][1] * matrix[3][3]  -  matrix[3][1] * matrix[2][3];
	f32 sub_factor_02 =  matrix[2][1] * matrix[3][2]  -  matrix[3][1] * matrix[2][2];
	f32 sub_factor_03 =  matrix[2][0] * matrix[3][3]  -  matrix[3][0] * matrix[2][3];
	f32 sub_factor_04 =  matrix[2][0] * matrix[3][2]  -  matrix[3][0] * matrix[2][2];
	f32 sub_factor_05 =  matrix[2][0] * matrix[3][1]  -  matrix[3][0] * matrix[2][1];

	Vector4_f32 determinant_coefficient {
		+ ( matrix[1][1] * sub_factor_00  -  matrix[1][2] * sub_factor_01  +  matrix[1][3] * sub_factor_02 ),
		- ( matrix[1][0] * sub_factor_00  -  matrix[1][2] * sub_factor_03  +  matrix[1][3] * sub_factor_04 ),
		+ ( matrix[1][0] * sub_factor_01  -  matrix[1][1] * sub_factor_03  +  matrix[1][3] * sub_factor_05 ),
		- ( matrix[1][0] * sub_factor_02  -  matrix[1][1] * sub_factor_04  +  matrix[1][2] * sub_factor_05 )
	};

	return  matrix[0][0] * determinant_coefficient[0] + matrix[0][1] * determinant_coefficient[1] +
	        matrix[0][2] * determinant_coefficient[2] + matrix[0][3] * determinant_coefficient[3];
}

Matrix4x4_f32 matrix4x4_f32_inverse(Matrix4x4_f32 matrix) {
	f32 coefficient_00 =  matrix[2][2] * matrix[3][3]  -  matrix[3][2] * matrix[2][3];
	f32 coefficient_02 =  matrix[1][2] * matrix[3][3]  -  matrix[3][2] * matrix[1][3];
	f32 coefficient_03 =  matrix[1][2] * matrix[2][3]  -  matrix[2][2] * matrix[1][3];

	f32 coefficient_04 =  matrix[2][1] * matrix[3][3]  -  matrix[3][1] * matrix[2][3];
	f32 coefficient_06 =  matrix[1][1] * matrix[3][3]  -  matrix[3][1] * matrix[1][3];
	f32 coefficient_07 =  matrix[1][1] * matrix[2][3]  -  matrix[2][1] * matrix[1][3];

	f32 coefficient_08 =  matrix[2][1] * matrix[3][2]  -  matrix[3][1] * matrix[2][2];
	f32 coefficient_10 =  matrix[1][1] * matrix[3][2]  -  matrix[3][1] * matrix[1][2];
	f32 coefficient_11 =  matrix[1][1] * matrix[2][2]  -  matrix[2][1] * matrix[1][2];

	f32 coefficient_12 =  matrix[2][0] * matrix[3][3]  -  matrix[3][0] * matrix[2][3];
	f32 coefficient_14 =  matrix[1][0] * matrix[3][3]  -  matrix[3][0] * matrix[1][3];
	f32 coefficient_15 =  matrix[1][0] * matrix[2][3]  -  matrix[2][0] * matrix[1][3];

	f32 coefficient_16 =  matrix[2][0] * matrix[3][2]  -  matrix[3][0] * matrix[2][2];
	f32 coefficient_18 =  matrix[1][0] * matrix[3][2]  -  matrix[3][0] * matrix[1][2];
	f32 coefficient_19 =  matrix[1][0] * matrix[2][2]  -  matrix[2][0] * matrix[1][2];

	f32 coefficient_20 =  matrix[2][0] * matrix[3][1]  -  matrix[3][0] * matrix[2][1];
	f32 coefficient_22 =  matrix[1][0] * matrix[3][1]  -  matrix[3][0] * matrix[1][1];
	f32 coefficient_23 =  matrix[1][0] * matrix[2][1]  -  matrix[2][0] * matrix[1][1];

	Vector4_f32 factor_0 = { coefficient_00, coefficient_00, coefficient_02, coefficient_03 };
	Vector4_f32 factor_1 = { coefficient_04, coefficient_04, coefficient_06, coefficient_07 };
	Vector4_f32 factor_2 = { coefficient_08, coefficient_08, coefficient_10, coefficient_11 };
	Vector4_f32 factor_3 = { coefficient_12, coefficient_12, coefficient_14, coefficient_15 };
	Vector4_f32 factor_4 = { coefficient_16, coefficient_16, coefficient_18, coefficient_19 };
	Vector4_f32 factor_5 = { coefficient_20, coefficient_20, coefficient_22, coefficient_23 };

	Vector4_f32 vector_0 = { matrix[1][0], matrix[0][0], matrix[0][0], matrix[0][0] };
	Vector4_f32 vector_1 = { matrix[1][1], matrix[0][1], matrix[0][1], matrix[0][1] };
	Vector4_f32 vector_2 = { matrix[1][2], matrix[0][2], matrix[0][2], matrix[0][2] };
	Vector4_f32 vector_3 = { matrix[1][3], matrix[0][3], matrix[0][3], matrix[0][3] };

	Vector4_f32 inverse_0 = { vector_1 * factor_0  -  vector_2 * factor_1  +  vector_3 * factor_2 };
	Vector4_f32 inverse_1 = { vector_0 * factor_0  -  vector_2 * factor_3  +  vector_3 * factor_4 };
	Vector4_f32 inverse_2 = { vector_0 * factor_1  -  vector_1 * factor_3  +  vector_3 * factor_5 };
	Vector4_f32 inverse_3 = { vector_0 * factor_2  -  vector_1 * factor_4  +  vector_2 * factor_5 };

	Vector4_f32 sign_a = { +1, -1, +1, -1 };
	Vector4_f32 sign_b = { -1, +1, -1, +1 };
	Matrix4x4_f32 inverse = {
		inverse_0 * sign_a,
		inverse_1 * sign_b,
		inverse_2 * sign_a,
		inverse_3 * sign_b
	};

	Vector4_f32 row_0 = { inverse[0][0], inverse[1][0], inverse[2][0], inverse[3][0] };

	Vector4_f32 dot_0 = { matrix[0] * row_0 };
	f32 dot_1 = ( dot_0.x + dot_0.y )  +  ( dot_0.z + dot_0.w );

	f32 one_over_determinant = 1.0f / dot_1;

	return inverse * one_over_determinant;
}

Matrix4x4_f32 matrix4x4_f32_multiply(Matrix4x4_f32 lhs, Matrix4x4_f32 rhs) {
	Matrix4x4_f32 result;
	result[0] =  lhs[0] * rhs[0][0]  +  lhs[1] * rhs[0][1]  +  lhs[2] * rhs[0][2]  + lhs[3] * rhs[0][3];
	result[1] =  lhs[0] * rhs[1][0]  +  lhs[1] * rhs[1][1]  +  lhs[2] * rhs[1][2]  + lhs[3] * rhs[1][3];
	result[2] =  lhs[0] * rhs[2][0]  +  lhs[1] * rhs[2][1]  +  lhs[2] * rhs[2][2]  + lhs[3] * rhs[2][3];
	result[3] =  lhs[0] * rhs[3][0]  +  lhs[1] * rhs[3][1]  +  lhs[2] * rhs[3][2]  + lhs[3] * rhs[3][3];
	return result;
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Matrix4x4_f32
//-----------------------------------------------------------------------------

Matrix4x4_f32::Matrix4x4_f32() {
	this->columns[0] = Vector4_f32 { 1, 0, 0, 0 };
	this->columns[1] = Vector4_f32 { 0, 1, 0, 0 };
	this->columns[2] = Vector4_f32 { 0, 0, 1, 0 };
	this->columns[3] = Vector4_f32 { 0, 0, 0, 1 };
}

Matrix4x4_f32::Matrix4x4_f32(f32 value) {
	this->columns[0] = Vector4_f32 { value,   0,     0,     0   };
	this->columns[1] = Vector4_f32 {   0,   value,   0,     0   };
	this->columns[2] = Vector4_f32 {   0,     0,   value,   0   };
	this->columns[3] = Vector4_f32 {   0,     0,     0,   value };
}

Matrix4x4_f32::Matrix4x4_f32(Vector4_f32 column0, Vector4_f32 column1, Vector4_f32 column2, Vector4_f32 column3) {
	this->columns[0] = column0;
	this->columns[1] = column1;
	this->columns[2] = column2;
	this->columns[3] = column3;
}

bool Matrix4x4_f32::operator == (Matrix4x4_f32 rhs) {
	return this->columns[0] == rhs[0] &&
	       this->columns[1] == rhs[1] &&
	       this->columns[2] == rhs[2] &&
	       this->columns[3] == rhs[3];
}

bool Matrix4x4_f32::operator == (f32 rhs) {
	return this->columns[0] == rhs &&
	       this->columns[1] == rhs &&
	       this->columns[2] == rhs &&
	       this->columns[3] == rhs;
}

Vector4_f32 & Matrix4x4_f32::operator [] (int index) {
	assert(index >= 0 && index < 4);
	return this->columns[index];
}

Matrix4x4_f32 & Matrix4x4_f32::operator += (Matrix4x4_f32 rhs) {
	this->columns[0] += rhs[0];
	this->columns[1] += rhs[1];
	this->columns[2] += rhs[2];
	this->columns[3] += rhs[3];
	return *this;
}

Matrix4x4_f32 & Matrix4x4_f32::operator -= (Matrix4x4_f32 rhs) {
	this->columns[0] -= rhs[0];
	this->columns[1] -= rhs[1];
	this->columns[2] -= rhs[2];
	this->columns[3] -= rhs[3];
	return *this;
}

Matrix4x4_f32 & Matrix4x4_f32::operator *= (Matrix4x4_f32 rhs) {
	*this = *this * rhs;
	return *this;
}

Matrix4x4_f32 & Matrix4x4_f32::operator /= (Matrix4x4_f32 rhs) {
	*this *= matrix4x4_f32_inverse(rhs);
	return *this;
}

Matrix4x4_f32 & Matrix4x4_f32::operator += (f32 rhs) {
	this->columns[0] += rhs;
	this->columns[1] += rhs;
	this->columns[2] += rhs;
	this->columns[3] += rhs;
	return *this;
}

Matrix4x4_f32 & Matrix4x4_f32::operator -= (f32 rhs) {
	this->columns[0] -= rhs;
	this->columns[1] -= rhs;
	this->columns[2] -= rhs;
	this->columns[3] -= rhs;
	return *this;
}

Matrix4x4_f32 & Matrix4x4_f32::operator *= (f32 rhs) {
	this->columns[0] *= rhs;
	this->columns[1] *= rhs;
	this->columns[2] *= rhs;
	this->columns[3] *= rhs;
	return *this;
}

Matrix4x4_f32 & Matrix4x4_f32::operator /= (f32 rhs) {
	this->columns[0] /= rhs;
	this->columns[1] /= rhs;
	this->columns[2] /= rhs;
	this->columns[3] /= rhs;
	return *this;
}

Matrix4x4_f32 Matrix4x4_f32::operator + (Matrix4x4_f32 rhs) {
	return Matrix4x4_f32(
		this->columns[0] + rhs[0],
		this->columns[1] + rhs[1],
		this->columns[2] + rhs[2],
		this->columns[3] + rhs[3]
	);
}

Matrix4x4_f32 Matrix4x4_f32::operator - (Matrix4x4_f32 rhs) {
	return Matrix4x4_f32(
		this->columns[0] - rhs[0],
		this->columns[1] - rhs[1],
		this->columns[2] - rhs[2],
		this->columns[3] - rhs[3]
	);
}

Matrix4x4_f32 Matrix4x4_f32::operator * (Matrix4x4_f32 rhs) {
	Matrix4x4_f32 result;
	result[0] =  this->columns[0] * rhs[0][0]  +  this->columns[1] * rhs[0][1]  +  this->columns[2] * rhs[0][2]  + this->columns[3] * rhs[0][3];
	result[1] =  this->columns[0] * rhs[1][0]  +  this->columns[1] * rhs[1][1]  +  this->columns[2] * rhs[1][2]  + this->columns[3] * rhs[1][3];
	result[2] =  this->columns[0] * rhs[2][0]  +  this->columns[1] * rhs[2][1]  +  this->columns[2] * rhs[2][2]  + this->columns[3] * rhs[2][3];
	result[3] =  this->columns[0] * rhs[3][0]  +  this->columns[1] * rhs[3][1]  +  this->columns[2] * rhs[3][2]  + this->columns[3] * rhs[3][3];
	return result;
}

Matrix4x4_f32 Matrix4x4_f32::operator / (Matrix4x4_f32 rhs) {
	Matrix4x4_f32 result = { *this };
	result /= rhs;
	return result;
}

Matrix4x4_f32 Matrix4x4_f32::operator + (f32 rhs)  {
	return Matrix4x4_f32(
		this->columns[0] + rhs,
		this->columns[1] + rhs,
		this->columns[2] + rhs,
		this->columns[3] + rhs
	);
}

Matrix4x4_f32 Matrix4x4_f32::operator - (f32 rhs) {
	return Matrix4x4_f32(
		this->columns[0] - rhs,
		this->columns[1] - rhs,
		this->columns[2] - rhs,
		this->columns[3] - rhs
	);
}

Matrix4x4_f32 Matrix4x4_f32::operator * (f32 rhs) {
	return Matrix4x4_f32(
		this->columns[0] * rhs,
		this->columns[1] * rhs,
		this->columns[2] * rhs,
		this->columns[3] * rhs
	);
}

Matrix4x4_f32 Matrix4x4_f32::operator / (f32 rhs) {
	return Matrix4x4_f32(
		this->columns[0] / rhs,
		this->columns[1] / rhs,
		this->columns[2] / rhs,
		this->columns[3] / rhs
	);
}

//-----------------------------------------------------------------------------
// [SECTION] Matrix4x4_f64 matrix functions
//-----------------------------------------------------------------------------

Matrix4x4_f64 matrix4x4_f64_transpose(Matrix4x4_f64 matrix) {
	Matrix4x4_f64 transpose;

	transpose[0][0] = matrix[0][0];
	transpose[0][1] = matrix[1][0];
	transpose[0][2] = matrix[2][0];
	transpose[0][3] = matrix[3][0];

	transpose[1][0] = matrix[0][1];
	transpose[1][1] = matrix[1][1];
	transpose[1][2] = matrix[2][1];
	transpose[1][3] = matrix[3][1];

	transpose[2][0] = matrix[0][2];
	transpose[2][1] = matrix[1][2];
	transpose[2][2] = matrix[2][2];
	transpose[2][3] = matrix[3][2];

	transpose[3][0] = matrix[0][3];
	transpose[3][1] = matrix[1][3];
	transpose[3][2] = matrix[2][3];
	transpose[3][3] = matrix[3][3];

	return transpose;
}

f64 matrix4x4_f64_determinant(Matrix4x4_f64 matrix) {
	f64 sub_factor_00 =  matrix[2][2] * matrix[3][3]  -  matrix[3][2] * matrix[2][3];
	f64 sub_factor_01 =  matrix[2][1] * matrix[3][3]  -  matrix[3][1] * matrix[2][3];
	f64 sub_factor_02 =  matrix[2][1] * matrix[3][2]  -  matrix[3][1] * matrix[2][2];
	f64 sub_factor_03 =  matrix[2][0] * matrix[3][3]  -  matrix[3][0] * matrix[2][3];
	f64 sub_factor_04 =  matrix[2][0] * matrix[3][2]  -  matrix[3][0] * matrix[2][2];
	f64 sub_factor_05 =  matrix[2][0] * matrix[3][1]  -  matrix[3][0] * matrix[2][1];

	Vector4_f64 determinant_coefficient {
		+ ( matrix[1][1] * sub_factor_00  -  matrix[1][2] * sub_factor_01  +  matrix[1][3] * sub_factor_02 ),
		- ( matrix[1][0] * sub_factor_00  -  matrix[1][2] * sub_factor_03  +  matrix[1][3] * sub_factor_04 ),
		+ ( matrix[1][0] * sub_factor_01  -  matrix[1][1] * sub_factor_03  +  matrix[1][3] * sub_factor_05 ),
		- ( matrix[1][0] * sub_factor_02  -  matrix[1][1] * sub_factor_04  +  matrix[1][2] * sub_factor_05 )
	};

	return  matrix[0][0] * determinant_coefficient[0] + matrix[0][1] * determinant_coefficient[1] +
	        matrix[0][2] * determinant_coefficient[2] + matrix[0][3] * determinant_coefficient[3];
}

Matrix4x4_f64 matrix4x4_f64_inverse(Matrix4x4_f64 matrix) {
	f64 coefficient_00 =  matrix[2][2] * matrix[3][3]  -  matrix[3][2] * matrix[2][3];
	f64 coefficient_02 =  matrix[1][2] * matrix[3][3]  -  matrix[3][2] * matrix[1][3];
	f64 coefficient_03 =  matrix[1][2] * matrix[2][3]  -  matrix[2][2] * matrix[1][3];

	f64 coefficient_04 =  matrix[2][1] * matrix[3][3]  -  matrix[3][1] * matrix[2][3];
	f64 coefficient_06 =  matrix[1][1] * matrix[3][3]  -  matrix[3][1] * matrix[1][3];
	f64 coefficient_07 =  matrix[1][1] * matrix[2][3]  -  matrix[2][1] * matrix[1][3];

	f64 coefficient_08 =  matrix[2][1] * matrix[3][2]  -  matrix[3][1] * matrix[2][2];
	f64 coefficient_10 =  matrix[1][1] * matrix[3][2]  -  matrix[3][1] * matrix[1][2];
	f64 coefficient_11 =  matrix[1][1] * matrix[2][2]  -  matrix[2][1] * matrix[1][2];

	f64 coefficient_12 =  matrix[2][0] * matrix[3][3]  -  matrix[3][0] * matrix[2][3];
	f64 coefficient_14 =  matrix[1][0] * matrix[3][3]  -  matrix[3][0] * matrix[1][3];
	f64 coefficient_15 =  matrix[1][0] * matrix[2][3]  -  matrix[2][0] * matrix[1][3];

	f64 coefficient_16 =  matrix[2][0] * matrix[3][2]  -  matrix[3][0] * matrix[2][2];
	f64 coefficient_18 =  matrix[1][0] * matrix[3][2]  -  matrix[3][0] * matrix[1][2];
	f64 coefficient_19 =  matrix[1][0] * matrix[2][2]  -  matrix[2][0] * matrix[1][2];

	f64 coefficient_20 =  matrix[2][0] * matrix[3][1]  -  matrix[3][0] * matrix[2][1];
	f64 coefficient_22 =  matrix[1][0] * matrix[3][1]  -  matrix[3][0] * matrix[1][1];
	f64 coefficient_23 =  matrix[1][0] * matrix[2][1]  -  matrix[2][0] * matrix[1][1];

	Vector4_f64 factor_0 = { coefficient_00, coefficient_00, coefficient_02, coefficient_03 };
	Vector4_f64 factor_1 = { coefficient_04, coefficient_04, coefficient_06, coefficient_07 };
	Vector4_f64 factor_2 = { coefficient_08, coefficient_08, coefficient_10, coefficient_11 };
	Vector4_f64 factor_3 = { coefficient_12, coefficient_12, coefficient_14, coefficient_15 };
	Vector4_f64 factor_4 = { coefficient_16, coefficient_16, coefficient_18, coefficient_19 };
	Vector4_f64 factor_5 = { coefficient_20, coefficient_20, coefficient_22, coefficient_23 };

	Vector4_f64 vector_0 = { matrix[1][0], matrix[0][0], matrix[0][0], matrix[0][0] };
	Vector4_f64 vector_1 = { matrix[1][1], matrix[0][1], matrix[0][1], matrix[0][1] };
	Vector4_f64 vector_2 = { matrix[1][2], matrix[0][2], matrix[0][2], matrix[0][2] };
	Vector4_f64 vector_3 = { matrix[1][3], matrix[0][3], matrix[0][3], matrix[0][3] };

	Vector4_f64 inverse_0 = { vector_1 * factor_0  -  vector_2 * factor_1  +  vector_3 * factor_2 };
	Vector4_f64 inverse_1 = { vector_0 * factor_0  -  vector_2 * factor_3  +  vector_3 * factor_4 };
	Vector4_f64 inverse_2 = { vector_0 * factor_1  -  vector_1 * factor_3  +  vector_3 * factor_5 };
	Vector4_f64 inverse_3 = { vector_0 * factor_2  -  vector_1 * factor_4  +  vector_2 * factor_5 };

	Vector4_f64 sign_a = { +1, -1, +1, -1 };
	Vector4_f64 sign_b = { -1, +1, -1, +1 };
	Matrix4x4_f64 inverse = {
		inverse_0 * sign_a,
		inverse_1 * sign_b,
		inverse_2 * sign_a,
		inverse_3 * sign_b
	};

	Vector4_f64 row_0 = { inverse[0][0], inverse[1][0], inverse[2][0], inverse[3][0] };

	Vector4_f64 dot_0 = { matrix[0] * row_0 };
	f64 dot_1 = ( dot_0.x + dot_0.y )  +  ( dot_0.z + dot_0.w );

	f64 one_over_determinant = 1.0f / dot_1;

	return inverse * one_over_determinant;
}

Matrix4x4_f64 matrix4x4_f64_multiply(Matrix4x4_f64 lhs, Matrix4x4_f64 rhs) {
	Matrix4x4_f64 result;
	result[0] =  lhs[0] * rhs[0][0]  +  lhs[1] * rhs[0][1]  +  lhs[2] * rhs[0][2]  + lhs[3] * rhs[0][3];
	result[1] =  lhs[0] * rhs[1][0]  +  lhs[1] * rhs[1][1]  +  lhs[2] * rhs[1][2]  + lhs[3] * rhs[1][3];
	result[2] =  lhs[0] * rhs[2][0]  +  lhs[1] * rhs[2][1]  +  lhs[2] * rhs[2][2]  + lhs[3] * rhs[2][3];
	result[3] =  lhs[0] * rhs[3][0]  +  lhs[1] * rhs[3][1]  +  lhs[2] * rhs[3][2]  + lhs[3] * rhs[3][3];
	return result;
}

//-----------------------------------------------------------------------------
// [SECTION] Operator overloadings: Matrix4x4_f64
//-----------------------------------------------------------------------------

Matrix4x4_f64::Matrix4x4_f64() {
	this->columns[0] = Vector4_f64 { 1, 0, 0, 0 };
	this->columns[1] = Vector4_f64 { 0, 1, 0, 0 };
	this->columns[2] = Vector4_f64 { 0, 0, 1, 0 };
	this->columns[3] = Vector4_f64 { 0, 0, 0, 1 };
}

Matrix4x4_f64::Matrix4x4_f64(f64 value) {
	this->columns[0] = Vector4_f64 { value,   0,     0,     0   };
	this->columns[1] = Vector4_f64 {   0,   value,   0,     0   };
	this->columns[2] = Vector4_f64 {   0,     0,   value,   0   };
	this->columns[3] = Vector4_f64 {   0,     0,     0,   value };
}

Matrix4x4_f64::Matrix4x4_f64(Vector4_f64 column0, Vector4_f64 column1, Vector4_f64 column2, Vector4_f64 column3) {
	this->columns[0] = column0;
	this->columns[1] = column1;
	this->columns[2] = column2;
	this->columns[3] = column3;
}

bool Matrix4x4_f64::operator == (Matrix4x4_f64 rhs) {
	return this->columns[0] == rhs[0] &&
	       this->columns[1] == rhs[1] &&
	       this->columns[2] == rhs[2] &&
	       this->columns[3] == rhs[3];
}

bool Matrix4x4_f64::operator == (f64 rhs) {
	return this->columns[0] == rhs &&
	       this->columns[1] == rhs &&
	       this->columns[2] == rhs &&
	       this->columns[3] == rhs;
}

Vector4_f64 & Matrix4x4_f64::operator [] (int index) {
	assert(index >= 0 && index < 4);
	return this->columns[index];
}

Matrix4x4_f64 & Matrix4x4_f64::operator += (Matrix4x4_f64 rhs) {
	this->columns[0] += rhs[0];
	this->columns[1] += rhs[1];
	this->columns[2] += rhs[2];
	this->columns[3] += rhs[3];
	return *this;
}

Matrix4x4_f64 & Matrix4x4_f64::operator -= (Matrix4x4_f64 rhs) {
	this->columns[0] -= rhs[0];
	this->columns[1] -= rhs[1];
	this->columns[2] -= rhs[2];
	this->columns[3] -= rhs[3];
	return *this;
}

Matrix4x4_f64 & Matrix4x4_f64::operator *= (Matrix4x4_f64 rhs) {
	*this = *this * rhs;
	return *this;
}

Matrix4x4_f64 & Matrix4x4_f64::operator /= (Matrix4x4_f64 rhs) {
	*this *= matrix4x4_f64_inverse(rhs);
	return *this;
}

Matrix4x4_f64 & Matrix4x4_f64::operator += (f64 rhs) {
	this->columns[0] += rhs;
	this->columns[1] += rhs;
	this->columns[2] += rhs;
	this->columns[3] += rhs;
	return *this;
}

Matrix4x4_f64 & Matrix4x4_f64::operator -= (f64 rhs) {
	this->columns[0] -= rhs;
	this->columns[1] -= rhs;
	this->columns[2] -= rhs;
	this->columns[3] -= rhs;
	return *this;
}

Matrix4x4_f64 & Matrix4x4_f64::operator *= (f64 rhs) {
	this->columns[0] *= rhs;
	this->columns[1] *= rhs;
	this->columns[2] *= rhs;
	this->columns[3] *= rhs;
	return *this;
}

Matrix4x4_f64 & Matrix4x4_f64::operator /= (f64 rhs) {
	this->columns[0] /= rhs;
	this->columns[1] /= rhs;
	this->columns[2] /= rhs;
	this->columns[3] /= rhs;
	return *this;
}

Matrix4x4_f64 Matrix4x4_f64::operator + (Matrix4x4_f64 rhs) {
	return Matrix4x4_f64(
		this->columns[0] + rhs[0],
		this->columns[1] + rhs[1],
		this->columns[2] + rhs[2],
		this->columns[3] + rhs[3]
	);
}

Matrix4x4_f64 Matrix4x4_f64::operator - (Matrix4x4_f64 rhs) {
	return Matrix4x4_f64(
		this->columns[0] - rhs[0],
		this->columns[1] - rhs[1],
		this->columns[2] - rhs[2],
		this->columns[3] - rhs[3]
	);
}

Matrix4x4_f64 Matrix4x4_f64::operator * (Matrix4x4_f64 rhs) {
	Matrix4x4_f64 result;
	result[0] =  this->columns[0] * rhs[0][0]  +  this->columns[1] * rhs[0][1]  +  this->columns[2] * rhs[0][2]  + this->columns[3] * rhs[0][3];
	result[1] =  this->columns[0] * rhs[1][0]  +  this->columns[1] * rhs[1][1]  +  this->columns[2] * rhs[1][2]  + this->columns[3] * rhs[1][3];
	result[2] =  this->columns[0] * rhs[2][0]  +  this->columns[1] * rhs[2][1]  +  this->columns[2] * rhs[2][2]  + this->columns[3] * rhs[2][3];
	result[3] =  this->columns[0] * rhs[3][0]  +  this->columns[1] * rhs[3][1]  +  this->columns[2] * rhs[3][2]  + this->columns[3] * rhs[3][3];
	return result;
}

Matrix4x4_f64 Matrix4x4_f64::operator / (Matrix4x4_f64 rhs) {
	Matrix4x4_f64 result = { *this };
	result /= rhs;
	return result;
}

Matrix4x4_f64 Matrix4x4_f64::operator + (f64 rhs)  {
	return Matrix4x4_f64(
		this->columns[0] + rhs,
		this->columns[1] + rhs,
		this->columns[2] + rhs,
		this->columns[3] + rhs
	);
}

Matrix4x4_f64 Matrix4x4_f64::operator - (f64 rhs) {
	return Matrix4x4_f64(
		this->columns[0] - rhs,
		this->columns[1] - rhs,
		this->columns[2] - rhs,
		this->columns[3] - rhs
	);
}

Matrix4x4_f64 Matrix4x4_f64::operator * (f64 rhs) {
	return Matrix4x4_f64(
		this->columns[0] * rhs,
		this->columns[1] * rhs,
		this->columns[2] * rhs,
		this->columns[3] * rhs
	);
}

Matrix4x4_f64 Matrix4x4_f64::operator / (f64 rhs) {
	return Matrix4x4_f64(
		this->columns[0] / rhs,
		this->columns[1] / rhs,
		this->columns[2] / rhs,
		this->columns[3] / rhs
	);
}