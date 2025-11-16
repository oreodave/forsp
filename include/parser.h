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

#ifndef PARSER_H
#define PARSER_H

#include "base.h"
#include "obj.h"

typedef struct
{
  const char *name;
  u64 position, size;
  char *buffer;

  obj_t *read_stack;
} stream_t;

stream_t stream_init_file(const char *filename);
void stream_delete(stream_t *);

obj_t *stream_read(state_t *, stream_t *);
vec_t stream_read_all(state_t *, stream_t *);

bool stream_at_end(stream_t *);
u64 stream_unread(stream_t *);
char stream_peek(stream_t *);
bool stream_forward(stream_t *, u64);
bool stream_backward(stream_t *, u64);

#endif
