/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 * Created: 2025-11-16
 * Description:
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base.h"
#include "obj.h"
#include "tag.h"
#include "vec.h"

obj_t *make_int(i64 i)
{
  return TAG(i, INT);
}

obj_t *make_pair(pair_t p)
{
  pair_t *new_pair = calloc(1, sizeof(*new_pair));
  *new_pair        = p;
  return TAG(new_pair, PAIR);
}

i64 as_int(obj_t *obj)
{
  assert(IS_INT(obj));
  u64 num = UNTAG(obj, INT);

  // Make sure we propagate the MSB across
  u8 msb = num >> 62 & 0b01;
  if (msb)
  {
    num |= (1LU << 63);
  }
  return (i64)num;
}

char *as_sym(obj_t *obj)
{
  assert(IS_SYM(obj));
  return (char *)UNTAG(obj, SYM);
}

pair_t *as_pair(obj_t *obj)
{
  assert(IS_PAIR(obj));
  return (pair_t *)UNTAG(obj, SYM);
}

obj_t *car(obj_t *obj)
{
  if (!IS_PAIR(obj))
    FAIL("car: Expected pair, got %p\n", obj);
  return as_pair(obj)->car;
}

obj_t *cdr(obj_t *obj)
{
  if (!IS_PAIR(obj))
    FAIL("cdr: Expected pair, got %p\n", obj);
  return as_pair(obj)->cdr;
}

obj_t *env_find(obj_t *env, obj_t *key)
{
  if (!IS_SYM(key))
    FAIL("env_find: Expected a symbol for KEY, got (%p)\n", key);
  for (obj_t *iter = env; iter; iter = cdr(iter))
  {
    assert(IS_PAIR(iter));
    obj_t *item = car(iter);
    if (car(item) == key)
      return cdr(item);
  }
  return NIL;
}

obj_t *env_set(state_t *state, obj_t *env, obj_t *key, obj_t *value)
{
  return cons(state, cons(state, key, value), env);
}

bool obj_equal(obj_t *a, obj_t *b)
{
  if (a == NIL && b == NIL)
    return true;
  else if ((IS_INT(a) && IS_INT(b)) || (IS_SYM(a) && IS_SYM(b)))
    return a == b;
  else if (IS_PAIR(a) && IS_PAIR(b))
  {
    if (a == b)
      return true;
    return obj_equal(car(a), car(b)) && obj_equal(cdr(a), cdr(b));
  }
  else
    return false;
}

obj_t *obj_copy(state_t *state, obj_t *obj)
{
  static_assert(NUM_TYPES == 4, "obj_copy implemented for 4 types of object.");
  if (IS_NIL(obj) || IS_INT(obj) || IS_SYM(obj))
  {
    return obj;
  }
  else if (IS_PAIR(obj))
  {
    // Not a deep clone, so just copy the container
    return cons(state, car(obj), cdr(obj));
  }
  else
  {
    FAIL("obj_copy: unexpected object(%p), not tagged\n", obj);
  }
}

obj_t *obj_clone(state_t *state, obj_t *obj)
{
  static_assert(NUM_TYPES == 4, "obj_clone implemented for 4 types of object.");
  if (IS_NIL(obj) || IS_INT(obj) || IS_SYM(obj))
  {
    return obj;
  }
  else if (IS_PAIR(obj))
  {
    // Not a deep clone, so just copy the container
    return cons(state, obj_clone(state, car(obj)), obj_clone(state, cdr(obj)));
  }
  else
  {
    FAIL("obj_clone: unexpected object(%p), not tagged\n", obj);
  }
}

void obj_string(obj_t *obj, vec_t *vec)
{
  static_assert(NUM_TYPES == 4,
                "obj_string implemented for 4 types of object.");
  if (IS_NIL(obj))
  {
    vec_append(vec, "()", 2);
  }
  else if (IS_INT(obj))
  {
    i64 n = as_int(obj);
    char buffer[snprintf(NULL, 0, "%lu", n) + 1];
    sprintf(buffer, "%lu", n);
    vec_append(vec, buffer, sizeof(buffer) - 1);
  }
  else if (IS_SYM(obj))
  {
    char *sym = as_sym(obj);
    vec_append(vec, sym, strlen(sym));
  }
  else if (IS_PAIR(obj))
  {
    vec_append(vec, "(", 1);
    for (obj_t *iter = obj; iter; iter = cdr(iter))
    {
      if (!IS_PAIR(iter))
      {
        obj_string(iter, vec);
        break;
      }

      obj_string(car(iter), vec);

      if (cdr(iter) == NIL)
        break;
      else if (IS_PAIR(cdr(iter)))
      {
        vec_append(vec, " ", 1);
      }
      else
      {
        vec_append(vec, " . ", 3);
      }
    }
    vec_append(vec, ")", 1);
  }
  else
  {
    FAIL("obj_string: unexpected object(%p), not tagged\n", obj);
  }
}

obj_t *intern(state_t *state, char *str, u64 size)
{
  assert(state);
  // Search the symbol cache first
  obj_t **ptrs = (void *)vec_data(&state->symcache);
  for (u64 i = 0; i < state->symcache.size / sizeof(*ptrs); ++i)
  {
    obj_t *o_sym = ptrs[i];
    assert(IS_SYM(o_sym));
    char *sym = as_sym(o_sym);
    if (strlen(sym) == size && memcmp(sym, str, size) == 0)
      // We found it!
      return o_sym;
  }

  // Otherwise, make a new symbol
  char *new_sym = calloc(size + 1, 1);
  memcpy(new_sym, str, size);
  new_sym[size]    = '\0';
  obj_t *o_new_sym = TAG(new_sym, SYM);

  // Add it to the cache for future lookup
  vec_append(&state->symcache, &o_new_sym, sizeof(o_new_sym));

  return o_new_sym;
}

obj_t *cons(state_t *state, obj_t *car, obj_t *cdr)
{
  pair_t p    = {car, cdr};
  obj_t *pair = make_pair(p);
  // Add pair to the list of allocations (for tracking)
  vec_append(&state->memory, &pair, sizeof(pair));
  return pair;
}

obj_t *make_list(state_t *state, obj_t **ptrs, u64 num)
{
  obj_t *root = NIL;
  for (u64 i = num; i > 0; --i)
  {
    root = cons(state, ptrs[i - 1], root);
  }
  return root;
}

void state_delete(state_t *state)
{
  assert(state);

  obj_t **ptrs = (void *)vec_data(&state->symcache);
  for (u64 i = 0; i < state->symcache.size / sizeof(*ptrs); ++i)
  {
    obj_t *o_sym = ptrs[i];
    assert(IS_SYM(o_sym));
    char *sym = as_sym(o_sym);
    if (sym)
      free(sym);
  }
  vec_delete(&state->symcache);

  ptrs = (void *)vec_data(&state->memory);
  for (u64 i = 0; i < state->memory.size / sizeof(*ptrs); ++i)
  {
    obj_t *o_pair = ptrs[i];
    assert(IS_PAIR(o_pair));
    pair_t *pair = as_pair(o_pair);
    if (pair)
      free(pair);
  }
  vec_delete(&state->memory);
}
