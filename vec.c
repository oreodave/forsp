/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 * Created: 2025-11-12
 * Description: Vector implementation
 */

#include <malloc.h>
#include <stdbool.h>
#include <string.h>

#include "vec.h"

void vec_init(vec_t *vec, u64 capacity)
{
  assert(vec);
  memset(vec, 0, sizeof(*vec));
  vec->size = 0;
  if (capacity <= VEC_CAPACITY_INLINE)
  {
    vec->capacity                  = VEC_CAPACITY_INLINE;
    vec->data.inlined.flag_inlined = true;
  }
  else
  {
    vec->capacity                  = capacity;
    vec->data.inlined.flag_inlined = false;
    vec->data.heap                 = calloc(1, vec->capacity);
  }
}

bool vec_inlined(vec_t *vec)
{
  return vec && vec->data.inlined.flag_inlined;
}

void *vec_data(vec_t *vec)
{
  if (vec_inlined(vec))
  {
    return vec->data.inlined.mem;
  }
  else
  {
    return vec->data.heap;
  }
}

void vec_ensure(vec_t *vec, u64 size)
{
  assert(vec);
  if (vec->capacity >= size)
  {
    return;
  }
  vec->capacity = MAX(vec->capacity * VEC_MULT, size);
  if (vec->capacity > VEC_CAPACITY_INLINE && vec_inlined(vec))
  {
    // Turn off inlining
    vec->data.inlined.flag_inlined = false;

    // Create a separate heap buffer and copy our work so far over
    u8 *buffer = calloc(1, vec->capacity);
    if (vec->size > 0)
      memcpy(buffer, vec->data.inlined.mem, vec->size);
    vec->data.heap = buffer;
  }
  else if (!vec_inlined(vec))
  {
    vec->data.heap = realloc(vec->data.heap, vec->capacity);
  }
}

void *vec_append(vec_t *vec, const void *const ptr, u64 size)
{
  assert(vec);
  vec_ensure(vec, vec->size + size);
  void *vec_ptr = vec_data(vec) + vec->size;
  memcpy(vec_ptr, ptr, size);
  vec->size += size;
  return vec_ptr;
}

void vec_delete(vec_t *vec)
{
  assert(vec);
  if (!vec_inlined(vec))
    free(vec->data.heap);

  memset(vec, 0, sizeof(*vec));
}
