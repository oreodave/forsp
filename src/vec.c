/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the Unlicense
 * for details.

 * You may distribute and modify this code under the terms of the
 * Unlicense, which you should have received a copy of along with this
 * program.  If not, please go to <https://unlicense.org/>.

 * Created: 2025-11-15
 * Description:
 */

#include <assert.h>
#include <malloc.h>
#include <string.h>

#include "vec.h"

void vec_ensure_capacity(vec_t *vec, u64 size)
{
  assert(vec);
  if (vec->capacity < size)
  {
    vec->capacity = MAX(vec->capacity * VEC_MULT, size);
    if (vec->capacity <= VEC_INLINE_CAPACITY)
    {
      return;
    }
    else if (VEC_INLINED(vec))
    {
      // Need to allocate on the heap then copy over our inlined stuff
      u8 *buffer = calloc(1, vec->capacity);
      memcpy(buffer, vec->stack.data, vec->size);
      vec->stack.is_not_inlined = true;
      vec->heap                 = buffer;
    }
    else
    {
      vec->heap = realloc(vec->heap, vec->capacity);
    }
  }
}

u64 vec_append(vec_t *vec, void *data, u64 size)
{
  assert(vec);
  assert(data);
  assert(size);

  vec_ensure_capacity(vec, vec->size + size);
  memcpy(vec_data(vec) + vec->size, data, size);

  u64 ptr = vec->size;
  vec->size += size;
  return ptr;
}

void *vec_data(vec_t *vec)
{
  assert(vec);

  if (VEC_INLINED(vec))
  {
    return vec->stack.data;
  }
  else
  {
    return vec->heap;
  }
}

void vec_delete(vec_t *vec)
{
  assert(vec);
  if (!VEC_INLINED(vec))
  {
    free(vec->heap);
  }
}
