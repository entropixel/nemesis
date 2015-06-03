/* See LICENSE file for copyright and licensing details */
// wrapper for stdint

#ifndef INT_H__
#define INT_H__
#include <stdint.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef int16_t fixed;

#define FRAC 4 // 16 sub-units
fixed float_to_fixed (float a);
float fixed_to_float (fixed a);
fixed fixmul (fixed a, fixed b);
fixed fixdiv (fixed a, fixed b);

#endif // INT_H__
