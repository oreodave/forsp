/* gc.c: Allocator and Garbage Collector implementation
 * Created: 2026-06-19
 * Author: Aryadev Chavali
 * License: See end of file
 */

#include "gc.h"

page_t *page_make()
{
  page_t *page =
      calloc(1, sizeof(*page) + (sizeof(*page->data) * PAGE_INIT_CAPACITY));
  page->length   = 0;
  page->capacity = PAGE_INIT_CAPACITY;
  return page;
}

pair_t *page_alloc(page_t *page)
{
  if (page->capacity - page->length == 0)
  {
    return NULL;
  }
  return &page->data[page->length++];
}

bool page_resize(page_t **page, u64 new_cap)
{
  if (!page || !*page || page[0]->capacity >= new_cap)
    return false;
  page[0] =
      realloc(page[0], sizeof(*page[0]) + (sizeof(*page[0]->data) * new_cap));
  return true;
}

void gc_init(gc_t *gc)
{
  gc->backup  = page_make();
  gc->current = page_make();
}

void gc_stop(gc_t *gc)
{
  free(gc->backup);
  free(gc->current);
}

pair_t *gc_alloc(gc_t *gc)
{
  pair_t *pair = page_alloc(gc->current);
  while (!pair)
  {
    // Collect and try again.
    gc_collect();
    pair = page_alloc(gc->current);

    if (!pair)
    {
      // If allocation has failed following collection, we need to increase the
      // size of our backup page and try again.
      page_resize(&gc->backup, gc->backup->capacity * 2);
    }
  }

  return pair;
}

void gc_collect()
{
  // TODO: complete algorithm
  FAIL("Not done");
}

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the MIT License for details.

 * You may distribute and modify this code under the terms of the MIT License,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://opensource.org/license/MIT>.

 */
