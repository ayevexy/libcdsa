#ifndef LIBCDSA_TYPES_H
#define LIBCDSA_TYPES_H

#include <stddef.h>
#include <stdint.h>

/**
 * @file types.h
 * @brief Optioned type aliases for existing c types.
 */

typedef uint8_t byte;
typedef size_t bytes;

typedef intptr_t intptr;
typedef uintptr_t uintptr;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float float32; // not guaranteed
typedef double float64; // not guaranteed

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef size_t usize;
typedef ptrdiff_t isize;

#endif
