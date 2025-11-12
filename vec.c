/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 * Created: 2025-11-12
 * Description:
 */

#include "vec.h"

#include <malloc.h>
#include <string.h>

void *vec_data(vec_t *vec)
{
  if (VEC_INLINED(vec))
    return vec->data.inlined;
  else
    return vec->data.payload;
}

void vec_ensure(vec_t *vec, u64 size)
{
  if (!vec || size < vec->capacity)
    return;
  vec->capacity = MAX(vec->capacity + size, vec->capacity * VEC_MULT);
  if (VEC_INLINED(vec))
    return;
  else if (!vec->data.payload)
  {
    // Need to migrate from the inline store to a separate pointer
    // FIXME: What if this fails?
    vec->data.payload = calloc(1, vec->capacity);
    if (vec->size > 0)
    {
      memcpy(vec->data.payload, vec->data.inlined, vec->size);
      memset(vec->data.inlined, 0, VEC_INLINE_CAPACITY);
    }
  }
  else
  {
    // FIXME: What if this fails?
    vec->data.payload = realloc(vec->data.payload, vec->capacity);
  }
}

u8 *vec_append(vec_t *vec, void *data, u64 size)
{
  vec_ensure(vec, vec->size + size);
  void *ptr = vec_data(vec) + vec->size;
  memcpy(ptr, data, size);
  vec->size += size;
  return ptr;
}

void vec_delete(vec_t *vec)
{
  if (!VEC_INLINED(vec))
  {
    free(vec->data.payload);
  }
  memset(vec, 0, sizeof(*vec));
}
