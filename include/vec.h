/* vec.h: Dynamic arrays for use throughout the runtime
 * Created: 2025-11-15
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#ifndef VEC_H
#define VEC_H

#include <stdbool.h>

#include "base.h"

#define VEC_INLINE_CAPACITY 47
#define VEC_MULT            2

typedef struct
{
  u8 data[VEC_INLINE_CAPACITY];
  bool is_not_inlined;
} vec_inline_t;

typedef struct
{
  u64 size, capacity;
  union
  {
    vec_inline_t stack;
    u8 *heap;
  };
} vec_t;

#define VEC_INLINED(V) (!(V)->stack.is_not_inlined)

void vec_ensure_capacity(vec_t *, u64);
u64 vec_append(vec_t *, void *, u64);
void *vec_data(vec_t *);
void vec_delete(vec_t *);

#endif

/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 */
