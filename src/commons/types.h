#ifndef TYPES_H
#define TYPES_H

#include "sys/types.h"
#include <stdbool.h>
#include <stddef.h>

typedef u_int64_t uint64;
typedef u_int32_t uint32;
typedef u_int16_t uint16;
typedef u_int8_t uint8;

typedef u_char byte;

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#endif // TYPES_H