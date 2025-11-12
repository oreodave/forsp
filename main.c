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
#include "vec.h"

int main(void)
{
  char str[] = "Hello, world!";
  vec_t vec  = {0};

  vec_append(&vec, str, sizeof(str));
  printf("%.*s\n", (int)vec.size, (char *)vec_data(&vec));

  return 0;
}
