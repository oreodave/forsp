/* base.h: Basic definitions
 * Created: 2025-11-15
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#ifndef BASE_H
#define BASE_H

#include <assert.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#define MAX(A, B)  ((A) > (B) ? (A) : (B))
#define MIN(A, B)  ((A) < (B) ? (A) : (B))
#define ARRSIZE(A) (sizeof(A) / sizeof((A)[0]))

#define TODO(MSG) (assert(0 && (MSG)))
#define FAIL(MSG, ...)                          \
  do                                            \
  {                                             \
    fprintf(stderr, "FAIL: " MSG, __VA_ARGS__); \
    exit(1);                                    \
  } while (0)

#endif

/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 */
