/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 * Created: 2025-11-13
 * Description: Generic streams of data (for parsing)
 */

#ifndef STREAM_H
#define STREAM_H

#include "base.h"
#include "vec.h"

#include <stdio.h>

typedef struct
{
  char *data;
  u64 size;
} sv_t;

#define SV_FMT(SV) (int)(SV).size, (SV).data
#define PR_SV      "%.*s"

// TODO: Make a general pipe based stream generator
// void stream_init_pipe(stream_t *, FILE *);
typedef struct
{
  const char *name;
  u64 position;
  vec_t data;
} stream_t;

void stream_init_cstr(stream_t *stream, const char *name, const sv_t sv);
void stream_init_file(stream_t *stream, const char *name, FILE *fp);
void stream_delete(stream_t *stream);
bool stream_eos(stream_t *stream);
char stream_peek(stream_t *stream);
bool stream_seek_forward(stream_t *stream, u64 offset);
bool stream_seek_backward(stream_t *stream, u64 offset);
sv_t stream_while(stream_t *stream, bool (*fn)(char));
sv_t stream_till(stream_t *stream, bool (*fn)(char));
sv_t stream_spn(stream_t *stream, char *accept);
sv_t stream_cspn(stream_t *stream, char *reject);
void stream_skip_whitespace(stream_t *stream);

#endif
