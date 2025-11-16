/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the Unlicense
 * for details.

 * You may distribute and modify this code under the terms of the
 * Unlicense, which you should have received a copy of along with this
 * program.  If not, please go to <https://unlicense.org/>.

 * Created: 2025-11-16
 * Description:
 */

#ifndef OBJ_H
#define OBJ_H

#include "base.h"
#include "tag.h"
#include "vec.h"

typedef struct
{
  obj_t *car, *cdr;
} pair_t;

typedef struct
{
  obj_t *body, *env;
} closure_t;

typedef struct
{
  void (*function)(obj_t *env);
} primitive_t;

typedef struct
{
  vec_t memory;
  vec_t symcache;

  // Value stack
  obj_t *stack;
  // Environment
  obj_t *env;
} state_t;

// Untracked by state
obj_t *make_int(i64);
obj_t *make_pair(pair_t);
i64 as_int(obj_t *);
char *as_sym(obj_t *);
pair_t *as_pair(obj_t *);

obj_t *car(obj_t *);
obj_t *cdr(obj_t *);
bool obj_equal(obj_t *, obj_t *);

void obj_string(obj_t *, vec_t *);

// Tracked by state
obj_t *intern(state_t *, char *, u64);
obj_t *cons(state_t *, obj_t *, obj_t *);
obj_t *make_list(state_t *, obj_t **, u64);

void state_delete(state_t *);

#endif
