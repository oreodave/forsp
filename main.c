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

#include <stdio.h>

#include "base.h"
#include "stream.h"
#include "vec.h"

int main(void)
{
  // test 1) string content
  char str[] =
      "Aliquam erat volutpat.\n"
      "Nunc eleifend leo vitae magna.\n"
      "In id erat non orci commodo lobortis.\n"
      "Proin neque massa, cursus ut, gravida ut, lobortis eget, lacus.\n"
      "Sed diam.\n"
      "Praesent fermentum tempor tellus.\n"
      "Nullam tempus.\n"
      "Mauris ac felis vel velit tristique imperdiet.\n"
      "Donec at pede.\n"
      "Etiam vel neque nec dui dignissim bibendum.\n"
      "Vivamus id enim.\n"
      "Phasellus neque orci, porta a, aliquet quis, semper a, massa.\n"
      "Phasellus purus.\n"
      "Pellentesque tristique imperdiet tortor.\n"
      "Nam euismod tellus id erat.\n";

  sv_t data       = {.data = str, .size = sizeof(str) - 1};
  stream_t stream = {0};

  stream_init_cstr(&stream, "<test>", data);

  for (u64 line_no = 1; !stream_eos(&stream); ++line_no)
  {
    stream_skip_whitespace(&stream);
    sv_t line = stream_cspn(&stream, "\n");
    printf("%s:%lu: " PR_SV "\n", stream.name, line_no, SV_FMT(line));
  }

  stream_delete(&stream);

  // test 2) files
  const char *name = "test.txt";
  FILE *fp         = fopen(name, "r");
  stream_init_file(&stream, name, fp);
  for (u64 line_no = 1; !stream_eos(&stream); ++line_no)
  {
    stream_skip_whitespace(&stream);
    sv_t line = stream_cspn(&stream, "\n");
    printf("%s:%lu: " PR_SV "\n", stream.name, line_no, SV_FMT(line));
  }
  stream_delete(&stream);
  fclose(fp);

  return 0;
}
