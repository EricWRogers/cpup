#pragma once

#include <stdint.h>

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t   u8; // 255
typedef uint16_t u16; // 65,535
typedef uint32_t u32; // 4,294,967,295
typedef uint64_t u64; // 18,446,744,073,709,551,615

#define u8_max		(255)
#define u16_max	    (65535)
#define u32_max	    (4294967295U)
#define u64_max     (18446744073709551615)

#define BIT(n) (1u << (n))

typedef float f32;
typedef double d64;