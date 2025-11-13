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

#include <assert.h>
#include <ctype.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "base.h"
#include "vec.h"

typedef struct
{
  char *data;
  u64 size;
} sv_t;

// TODO: Make a general pipe based stream generator
// void stream_init_pipe(stream_t *, FILE *);
typedef struct
{
  const char *name;
  u64 position;
  vec_t data;
} stream_t;

void stream_init_cstr(stream_t *stream, const char *name, const sv_t sv)
{
  assert(stream);
  assert(sv.data);
  stream->name     = name;
  stream->position = 0;
  u64 size = sv.size + (sv.size > 0 && sv.data[sv.size - 1] != '\0' ? 1 : 0);
  vec_init(&stream->data, size);
  vec_append(&stream->data, sv.data, sv.size);
  // Get that null terminator in bud.
  if (size > sv.size)
    vec_append(&stream->data, "\0", 1);
}

void stream_init_file(stream_t *stream, const char *name, FILE *fp)
{
  assert(stream);
  assert(fp);
  assert(!feof(fp));

  stream->name     = name;
  stream->position = 0;

  assert(!fseek(fp, 0, SEEK_SET));
  assert(!fseek(fp, 0, SEEK_END));
  long size = ftell(fp);
  assert(!fseek(fp, 0, SEEK_SET));

  vec_init(&stream->data, size + 1);
  fread(vec_data(&stream->data), size, 1, fp);
  stream->data.size = size;

  // Get that null terminator in bud.
  if (size > 0)
    vec_append(&stream->data, "\0", 1);
}

void stream_delete(stream_t *stream)
{
  assert(stream);
  vec_delete(&stream->data);
  memset(stream, 0, sizeof(*stream));
}

bool stream_eos(stream_t *stream)
{
  assert(stream);
  return stream->position + 1 >= stream->data.size;
}

char stream_peek(stream_t *stream)
{
  return stream_eos(stream)
             ? '\0'
             : ((char *)vec_data(&stream->data))[stream->position];
}

bool stream_seek_forward(stream_t *stream, u64 offset)
{
  if (stream_eos(stream))
    return false;
  if (stream->data.size >= stream->position + offset)
  {
    stream->position += offset;
    return true;
  }
  else
    return false;
}

bool stream_seek_backward(stream_t *stream, u64 offset)
{
  assert(stream);
  if (stream->position < offset)
    return false;
  stream->position -= offset;
  return true;
}

sv_t stream_while(stream_t *stream, bool (*fn)(char))
{
  assert(stream);
  u64 pos = stream->position;
  for (char c = stream_peek(stream); c != '\0' && fn(c);
       stream_seek_forward(stream, 1), c = stream_peek(stream))
    continue;
  u64 size = stream->position - pos;
  return (sv_t){vec_data(&stream->data) + pos, size};
}

sv_t stream_till(stream_t *stream, bool (*fn)(char))
{
  assert(stream);
  u64 pos = stream->position;
  for (char c = stream_peek(stream); c != '\0' && !fn(c);
       stream_seek_forward(stream, 1), c = stream_peek(stream))
    continue;
  u64 size = stream->position - pos;
  return (sv_t){.data = vec_data(&stream->data) + pos, .size = size};
}

sv_t stream_spn(stream_t *stream, char *accept)
{
  assert(stream);
  // FIXME: When streams occur, this won't work I don't think?
  u64 size = strspn(vec_data(&stream->data) + stream->position, accept);
  sv_t sv  = {.data = vec_data(&stream->data) + stream->position, size};
  stream->position += size;
  return sv;
}

sv_t stream_cspn(stream_t *stream, char *reject)
{
  assert(stream);
  // FIXME: When streams occur, this won't work I don't think?
  u64 size = strcspn(vec_data(&stream->data) + stream->position, reject);
  sv_t sv  = {.data = vec_data(&stream->data) + stream->position, size};
  stream->position += size;
  return sv;
}

bool is_space(char c)
{
  return isspace(c);
}

void stream_skip_whitespace(stream_t *stream)
{
  (void)stream_while(stream, is_space);
}

int main(void)
{
  char buffer[]   = " hhhh";
  sv_t data       = {.data = buffer, .size = sizeof(buffer) - 1};
  stream_t stream = {0};
  stream_init_cstr(&stream, "<test>", data);

  while (!stream_eos(&stream))
  {
    stream_skip_whitespace(&stream);
    sv_t line = stream_cspn(&stream, "\n");
    printf("%.*s", (int)line.size, line.data);
  }

  stream_delete(&stream);

  return 0;
}
