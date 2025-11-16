/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 * Created: 2025-11-15
 * Description:
 */

#ifndef TAG_H
#define TAG_H

#include "base.h"

#define NUM_TYPES 4

typedef enum
{
  TAG_NIL       = 0b00000000,
  TAG_INT       = 0b00000001,
  TAG_SYM       = 0b00000010,
  TAG_PAIR      = 0b00000100,
  TAG_CLOSURE   = 0b00000110,
  TAG_PRIMITIVE = 0b00001000,
} tag_t;

typedef enum
{
  SHIFT_NIL       = 0,
  SHIFT_INT       = 1,
  SHIFT_SYM       = 8,
  SHIFT_PAIR      = 8,
  SHIFT_CLOSURE   = 8,
  SHIFT_PRIMITIVE = 8,
} shift_t;

typedef struct TaggedPointer obj_t;
#define NIL ((obj_t *)NULL)

#define TAG(X, TYPE)     ((obj_t *)((((u64)(X)) << SHIFT_##TYPE) | TAG_##TYPE))
#define UNTAG(X, TYPE)   (((u64)(X)) >> SHIFT_##TYPE)
#define GET_TAG(X, TYPE) (((u64)(X)) & ((1 << SHIFT_##TYPE) - 1))

#define IS_NIL(X)       ((X) == NIL)
#define IS_INT(X)       (GET_TAG(X, INT) == TAG_INT)
#define IS_SYM(X)       (GET_TAG(X, SYM) == TAG_SYM)
#define IS_PAIR(X)      (GET_TAG(X, PAIR) == TAG_PAIR)
#define IS_CLOSURE(X)   (GET_TAG(X, CLOSURE) == TAG_CLOSURE)
#define IS_PRIMITIVE(X) (GET_TAG(X, PRIMITIVE) == TAG_PRIMITIVE)

#endif
