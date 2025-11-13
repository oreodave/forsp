/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 * Created: 2025-11-12
 * Description: Dynamic array (vector)
 */

#ifndef VEC_H
#define VEC_H

#include "base.h"

#define VEC_MULT            2
#define VEC_CAPACITY_INLINE 47

struct VecInline
{
  u8 mem[VEC_CAPACITY_INLINE];
  bool flag_inlined;
};

typedef struct
{
  u64 size, capacity;
  union
  {
    struct VecInline inlined;
    u8 *heap;
  } data;
} vec_t;

void vec_init(vec_t *vec, u64 capacity);
bool vec_inlined(vec_t *vec);
void *vec_data(vec_t *vec);
void vec_ensure(vec_t *vec, u64 size);
void *vec_append(vec_t *vec, const void *const ptr, u64 size);
void vec_delete(vec_t *vec);

#endif
