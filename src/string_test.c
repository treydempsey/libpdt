#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "class_String.h"

int main(int argc, char *argv[], char *envp[])
{
  String *a, *b;
  int i, j;

  class_String();

  a = new_String("Hello World", 8, 11);
  a->m->set_blk_size(a, 8);
  b = new_String("", 8, 0);

  printf(">>%s<<\n", a->string);
  for(i = 0; i < a->length; i++) {
    b->m->slice(b, a, 2);
    for(j = 0; j < i; j++) {
      printf(" ");
    }
    printf(">>%s<<\n", b->string);
    a->position++;
  }
  printf(">>%s<<\n", a->string);

  b->m->truncate(b);
  b->m->append_cstr(b, " A Test", 7);

  printf("b: %s\n", b->string);

  a->m->append(a, b);

  printf("a: %s\n", a->string);
  printf("a->size: %zu, length: %zu\n", a->size, a->length);


  a->m->free(a);
  b->m->free(b);

  return 0;
}
