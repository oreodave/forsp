/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 * Created: 2025-11-12
 * Description: Vectors
 */

#ifndef VEC_H
#define VEC_H

#include "base.h"

#define VEC_INLINE_CAPACITY 16
#define VEC_MULT            1.5
#define VEC_INLINED(V)      ((V) && (V)->capacity <= VEC_INLINE_CAPACITY)

typedef struct
{
  u64 size, capacity;
  union
  {
    u8 inlined[VEC_INLINE_CAPACITY];
    u8 *payload;
  } data;
} vec_t;

void *vec_data(vec_t *vec);
void vec_ensure(vec_t *vec, u64 size);
u8 *vec_append(vec_t *vec, void *data, u64 size);
void vec_delete(vec_t *vec);

#endif
