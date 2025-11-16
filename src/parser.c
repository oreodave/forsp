/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSITIONE.  See the Unlicense
 * for details.

 * You may distribute and modify this code under the terms of the
 * Unlicense, which you should have received a copy of along with this
 * program.  If not, please go to <https://unlicense.org/>.

 * Created: 2025-11-16
 * Description:
 */

#include <ctype.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base.h"
#include "obj.h"
#include "parser.h"
#include "vec.h"

static const char *ACCEPTABLE_SYM_CHARS =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ[]-=#';/"
    ".,+_}{~@:?><}!£%&*";

stream_t stream_init_file(const char *filename)
{
  stream_t contents = {0};

  FILE *fp = fopen(filename, "r");

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  contents.name     = filename;
  contents.size     = size;
  contents.position = 0;
  contents.buffer   = calloc(1, contents.size);
  fread(contents.buffer, size, 1, fp);

  fclose(fp);

  return contents;
}

void stream_delete(stream_t *stream)
{
  assert(stream);
  free(stream->buffer);
  memset(stream, 0, sizeof(*stream));
}

bool stream_at_end(stream_t *stream)
{
  return stream->position >= stream->size ||
         (stream->position == stream->size - 1 &&
          stream->buffer[stream->position] == '\n');
}

u64 stream_unread(stream_t *stream)
{
  if (stream->position >= stream->size)
    return 0;
  else
    return stream->size - stream->position;
}

char stream_peek(stream_t *stream)
{
  if (stream_at_end(stream))
    return '\0';
  else
    return stream->buffer[stream->position];
}

bool stream_forward(stream_t *stream, u64 offset)
{
  if (stream_unread(stream) < offset)
    return false;
  stream->position += offset;
  return true;
}

bool stream_backward(stream_t *stream, u64 offset)
{
  if (stream->position < offset)
    return false;
  stream->position -= offset;
  return true;
}

void stream_skip_whitespace(stream_t *stream)
{
  for (char c = stream_peek(stream); !stream_at_end(stream) && isspace(c);
       stream_forward(stream, 1), c = stream_peek(stream))
    continue;
}

void stream_skip_comment(stream_t *stream)
{
  if (stream_peek(stream) != ';')
    return;
  stream_forward(stream, 1);
  for (char c = stream_peek(stream);
       !stream_at_end(stream) && c != '\n' && c != '\r';
       stream_forward(stream, 1), c = stream_peek(stream))
    continue;
  stream_skip_whitespace(stream);
}

obj_t *stream_read_list(state_t *state, stream_t *stream)
{
  stream_forward(stream, 1);
  if (stream_at_end(stream))
  {
    // FIXME:Stream_Error: No close bracket
    FAIL("reader: Expected close bracket\n--> %s:[%lu]\n", stream->name,
         stream->position);
    return NIL;
  }

  u64 pos     = stream->position;
  vec_t items = {0};
  while (!stream_at_end(stream) && stream_peek(stream) != ')')
  {
    obj_t *ret = stream_read(state, stream);
    vec_append(&items, &ret, sizeof(ret));
    stream_skip_whitespace(stream);
  }

  if (stream_peek(stream) == ')')
    stream_forward(stream, 1);
  else
    FAIL("reader: Expected close bracket\n--> %s:[%lu]\n--> %s:[%lu]\n",
         stream->name, pos, stream->name, stream->position);

  // Create an actual list now
  obj_t **objects = (obj_t **)vec_data(&items);
  u64 size        = items.size / sizeof(*objects);
  obj_t *root     = make_list(state, objects, size);
  vec_delete(&items);

  return root;
}

obj_t *stream_read_number(stream_t *stream)
{
  u64 length = strspn(stream->buffer + stream->position, "0123456789");
  if (length == 0)
    FAIL("reader: Expected number to parse, read `%c`\n--> %s:[%lu]\n",
         stream_peek(stream), stream->name, stream->position);
  char buffer[length + 1];
  memcpy(buffer, stream->buffer + stream->position, length);
  buffer[length] = '\0';

  obj_t *obj = make_int(atoll(buffer));
  stream_forward(stream, length);
  return obj;
}

obj_t *stream_read_sym(state_t *state, stream_t *stream)
{
  u64 length = strspn(stream->buffer + stream->position, ACCEPTABLE_SYM_CHARS);
  if (length == 0)
    FAIL("reader: Expected symbol, read `%c`\n--> %s:[%lu]\n",
         stream_peek(stream), stream->name, stream->position);
  obj_t *obj = intern(state, stream->buffer + stream->position, length);
  stream_forward(stream, length);
  return obj;
}

obj_t *stream_read(state_t *state, stream_t *stream)
{
  stream_skip_whitespace(stream);
  if (stream_at_end(stream))
    // FIXME:Stream_Error: EOS
    return NIL;
  else if (stream->read_stack)
  {
    obj_t *item        = car(stream->read_stack);
    stream->read_stack = cdr(stream->read_stack);
    return item;
  }

  char c[2];
  c[0] = stream_peek(stream);
  c[1] = '\0';
  if (c[0] == ';')
  {
    stream_skip_comment(stream);
    // tail call
    return stream_read(state, stream);
  }
  else if (c[0] == '\'')
  {
    return intern(state, "quote", 5);
  }
  else if (c[0] == '^')
  {
    obj_t *items[3];
    items[0] = intern(state, "quote", 5);
    items[2] = intern(state, "push", 4);

    stream_forward(stream, 1);
    items[1]           = stream_read_sym(state, stream);
    stream->read_stack = make_list(state, items, ARRSIZE(items));
    return stream_read(state, stream);
  }
  else if (c[0] == '$')
  {
    obj_t *items[3];
    items[0] = intern(state, "quote", 5);
    items[2] = intern(state, "pop", 3);

    stream_forward(stream, 1);
    items[1]           = stream_read_sym(state, stream);
    stream->read_stack = make_list(state, items, ARRSIZE(items));
    return stream_read(state, stream);
  }
  else if (c[0] == '(')
  {
    return stream_read_list(state, stream);
  }
  else if (strspn(c, "0123456789") != 0)
  {
    return stream_read_number(stream);
  }
  else if (strspn(c, ACCEPTABLE_SYM_CHARS) != 0)
  {
    return stream_read_sym(state, stream);
  }
  else
  {
    FAIL("reader: Unknown character (%d=`%c`)\n--> %s:[%lu]\n", c[0], c[0],
         stream->name, stream->position);
    return NIL;
  }
}

vec_t stream_read_all(state_t *state, stream_t *stream)
{
  vec_t expressions = {0};
  while (!stream_at_end(stream))
  {
    obj_t *ret = stream_read(state, stream);
    vec_append(&expressions, &ret, sizeof(ret));
  }
  return expressions;
}
