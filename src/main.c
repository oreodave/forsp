/* main.c: Entrypoint
 * Created: 2025-11-15
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "base.h"
#include "obj.h"
#include "parser.h"
#include "tag.h"
#include "vec.h"

void test_intern(state_t *state)
{
  // TEST: Multiple Symbol lookup
  char *symbols[] = {
      "cl-loop",
      "+",
      "endif",
  };
  obj_t *symbol_pointers[ARRSIZE(symbols)];

  for (u64 i = 0; i < ARRSIZE(symbols); ++i)
  {
    char *sym   = symbols[i];
    obj_t *osym = intern(state, sym, strlen(sym));
    assert(IS_SYM(osym));
    printf("[TEST/intern]: Mapped `%s` -> %p\n", sym, osym);
    symbol_pointers[i] = osym;
  }

  for (u64 i = 0; i < ARRSIZE(symbols); ++i)
  {
    char *sym   = symbols[i];
    obj_t *osym = intern(state, sym, strlen(sym));
    assert(IS_SYM(osym));
    printf("[TEST/intern]: Expected %p, got %p\n", symbol_pointers[i], osym);
    assert(osym == symbol_pointers[i]);
  }
}

void test_cons(state_t *state)
{
  // simple test
  obj_t *a = make_int(1);
  obj_t *b = make_int(2);
  printf("[TEST/cons]: cons(%p, %p)", a, b);

  obj_t *root = cons(state, a, b);
  printf(" => %p", root);

  assert(IS_PAIR(root));
  pair_t *pair = as_pair(root);
  printf(" => %p", pair);

  assert(pair);
  printf(" => (%p, %p)\n", pair->car, pair->cdr);
  assert(pair->car == a && pair->cdr == b);

  // back to back cons'ing
  obj_t *expression[] = {
      NIL,
  };

  root = NIL;
  for (u64 i = 0; i < ARRSIZE(expression); ++i)
  {
    root = cons(state, expression[i], root);
    printf("[TEST/cons]: root=%p\n", root);
    assert(IS_PAIR(root));
  }

  vec_t vec = {0};
  obj_string(root, &vec);
  printf("[TEST/cons]: root = %.*s\n", (int)vec.size, (char *)vec_data(&vec));
}

int main(void)
{
  state_t state = {0};

  const char filename[] = "./examples/test.fp";
  stream_t stream       = stream_init_file(filename);
  printf("%s: %lu bytes read\n", filename, stream.size);

  vec_t vec_objects = stream_read_all(&state, &stream);
  stream_delete(&stream);

  obj_t *iter     = NIL;
  obj_t **objects = vec_data(&vec_objects);
  for (u64 i = 0; i < vec_objects.size / sizeof(iter); i++)
  {
    vec_t obj_str = {0};
    obj_string(objects[i], &obj_str);
    printf("(%lu) => %.*s\n", i, (int)obj_str.size, (char *)vec_data(&obj_str));
    vec_delete(&obj_str);
  }

  vec_delete(&vec_objects);
  state_delete(&state);
  return 0;
}

/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 */
