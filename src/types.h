#ifndef QLIGHT_TYPES_H
#define QLIGHT_TYPES_H

#include <stdint.h>

#define  S8_MAX     INT8_MAX
#define  U8_MAX    UINT8_MAX
#define S16_MAX    INT16_MAX
#define U16_MAX   UINT16_MAX
#define S32_MAX    INT32_MAX
#define U32_MAX   UINT32_MAX
#define S64_MAX    INT64_MAX
#define U64_MAX   UINT64_MAX

#define     F32_MAX  FLT_MAX
#define     F32_MIN  FLT_MIN
#define FLOAT32_MAX  FLT_MAX
#define FLOAT32_MIN  FLT_MIN
#define     F64_MAX  DBL_MAX
#define     F64_MIN  DBL_MIN
#define FLOAT64_MAX  DBL_MAX
#define FLOAT64_MIN  DBL_MIN

typedef   int8_t  s8;
typedef  uint8_t  u8;
typedef  int16_t  s16;
typedef uint16_t  u16;
typedef  int32_t  s32;
typedef uint32_t  u32;
typedef  int64_t  s64;
typedef uint64_t  u64;

typedef    float  f32;
typedef   double  f64;

template <typename T>
inline const char *get_type_name() { return "(unknown)"; }

template < >
inline const char *get_type_name<char>() { return "char"; }

template < >
inline const char *get_type_name<s8>() { return "s8"; }

template < >
inline const char *get_type_name<s16>() { return "s16"; }

template < >
inline const char *get_type_name<s32>() { return "s32"; }

template < >
inline const char *get_type_name<s64>() { return "s64"; }

template < >
inline const char *get_type_name<u8>() { return "u8"; }

template < >
inline const char *get_type_name<u16>() { return "u16"; }

template < >
inline const char *get_type_name<u32>() { return "u32"; }

template < >
inline const char *get_type_name<u64>() { return "u64"; }

template < >
inline const char *get_type_name<f32>() { return "f32"; }

template < >
inline const char *get_type_name<f64>() { return "f64"; }

#endif /* QLIGHT_TYPES_H */