#include "./lib/cgraph/cgraph.h"
#include <stdio.h>

int main(int argc, char **argv) {
  Agraph_t *g = agread(stdin, NULL);
  printf("Hello world!\n");
  printf("g = %p\n", g);
  agwrite(g, stdout);
  return 0;
}
