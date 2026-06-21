/* gc.c: Pool-based mark-sweep garbage collector for pairs and closures.
 * Created: 2026-06-21
 * Author: Aryadev Chavali
 * License: See end of file
 */

#include "gc.h"
#include "state.h"

void gc_reset(void)
{
  for (size_t i = 0; i < state->gc.pool.length; ++i)
  {
    free(state->gc.pool.chunks[i]);
  }
  free(state->gc.pool.chunks);
  memset(&state->gc, 0, sizeof(state->gc));
  state->gc.metadata.threshold = 64 * 16;
}

static inline void gc_free_list_push(void *slot)
{
  *(void **)slot      = state->gc.free_list;
  state->gc.free_list = slot;
}

static inline void *gc_free_list_pop()
{
  void *slot = state->gc.free_list;
  if (slot)
  {
    state->gc.free_list = *(void **)slot;
  }
  return slot;
}

/** Construct a new chunk and push it onto the pool.
 */
static gc_chunk_t *gc_new_chunk(void)
{
  // aligned_alloc(4096, ...) gives 16-byte alignment for slots
  gc_chunk_t *c = aligned_alloc(4096, sizeof(gc_chunk_t));
  if (!c)
  {
    FAIL("GC: failed to allocate chunk");
  }
  memset(c->mark_bits, 0, sizeof(c->mark_bits));
  memset(c->alloc_bits, 0, sizeof(c->alloc_bits));

  // Chain all new slots into the free list
  for (size_t i = 0; i < GC_CHUNK_SLOTS; ++i)
  {
    void *slot = c->data + i * 16;
    gc_free_list_push(slot);
  }

  // Push onto the chunk array in the pool.
  if (!state->gc.pool.capacity)
  {
    state->gc.pool.capacity = 1;
    state->gc.pool.chunks   = malloc(sizeof(*state->gc.pool.chunks));
  }
  else if (state->gc.pool.capacity - state->gc.pool.length == 0)
  {
    state->gc.pool.capacity *= 2;
    state->gc.pool.chunks =
        realloc(state->gc.pool.chunks,
                sizeof(*state->gc.pool.chunks) * state->gc.pool.capacity);
  }

  if (!state->gc.pool.chunks)
  {
    FAIL("GC: failed to reallocate pool of chunks");
  }

  state->gc.pool.chunks[state->gc.pool.length++] = c;

  return c;
}

/** Locate which chunk owns a raw pointer.
 */
static gc_chunk_t *gc_find_chunk(void *raw_ptr)
{
  u8 *raw = raw_ptr;
  for (size_t i = 0; i < state->gc.pool.length; ++i)
  {
    auto c     = state->gc.pool.chunks[i];
    auto start = c->data;
    auto end   = start + GC_CHUNK_DATA_SIZE;
    if (raw >= start && raw < end)
      return c;
  }
  return NULL;
}

/** Get the index slot index for a given pointer, for use in
 * mark_bits/alloc_bits.
 */
static size_t gc_slot_index(gc_chunk_t *c, void *raw)
{
  return ((u8 *)raw - c->data) / 16;
}

/** Bitmap helpers — work for both mark_bits and alloc_bits.
 */
static inline void gc_bit_set(u64 *bits, size_t idx)
{
  bits[idx / 64] |= (1ULL << (idx % 64));
}

static inline void gc_bit_clear(u64 *bits, size_t idx)
{
  bits[idx / 64] &= ~(1ULL << (idx % 64));
}

static inline bool gc_bit_test(const u64 *bits, size_t idx)
{
  return (bits[idx / 64] >> (idx % 64)) & 1;
}

__attribute__((noinline)) obj_t *gc_alloc(tag_t tag)
{
  if (state->gc.metadata.alloc_bytes >= state->gc.metadata.threshold)
  {
    gc_collect();
  }

  if (!state->gc.free_list)
    gc_new_chunk();

  void *slot = gc_free_list_pop();
  state->gc.metadata.alloc_live++;
  state->gc.metadata.alloc_bytes += 16;

  gc_chunk_t *c = gc_find_chunk(slot);
  size_t idx    = gc_slot_index(c, slot);
  gc_bit_set(c->alloc_bits, idx);

  return TAG_CANON(slot, tag);
}

void gc_mark_obj(obj_t *obj)
{
  if (!obj || IS_NIL(obj))
    return;

  if (!IS_ALLOC(obj))
    return;

  void *raw     = (void *)UNTAG(obj);
  gc_chunk_t *c = gc_find_chunk(raw);
  if (!c)
    return;

  size_t idx = gc_slot_index(c, raw);
  if (gc_bit_test(c->mark_bits, idx))
    return;

  gc_bit_set(c->mark_bits, idx);

  // pair_t and clos_t both start with two obj_t* fields
  obj_t **fields = (obj_t **)raw;
  gc_mark_obj(fields[0]);
  gc_mark_obj(fields[1]);
}

size_t gc_sweep(void)
{
  size_t freed = 0;
  for (size_t i = 0; i < state->gc.pool.length; ++i)
  {
    gc_chunk_t *c = state->gc.pool.chunks[i];
    for (size_t j = 0; j < GC_CHUNK_SLOTS; ++j)
    {
      if (!gc_bit_test(c->alloc_bits, j))
        continue;

      if (!gc_bit_test(c->mark_bits, j))
      {
        void *slot = c->data + j * 16;
        gc_free_list_push(slot);
        gc_bit_clear(c->alloc_bits, j);
        freed++;
      }
    }
    memset(c->mark_bits, 0, sizeof(c->mark_bits));
  }

  state->gc.metadata.alloc_live -= freed;
  state->gc.metadata.alloc_bytes = state->gc.metadata.alloc_live * 16;
  state->gc.metadata.threshold   = state->gc.metadata.alloc_bytes * 2;
  if (state->gc.metadata.threshold < 64 * 16)
    state->gc.metadata.threshold = 64 * 16;

  return freed;
}

size_t gc_collect(void)
{
  // FIXME: Implement marking of root objects once integrated into interpreter.
  return gc_sweep();
}

size_t gc_alloc_count(void)
{
  return state->gc.metadata.alloc_live;
}

size_t gc_bytes_allocated(void)
{
  return state->gc.metadata.alloc_bytes;
}

size_t gc_num_chunks(void)
{
  return state->gc.pool.length;
}

/* Copyright (c) 2024 Anthony Bonkoski
 * Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the MIT License for details.

 * You may distribute and modify this code under the terms of the MIT License,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://opensource.org/license/MIT>.

 */
