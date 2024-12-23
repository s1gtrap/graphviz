#include "./lib/cgraph/cgraph.h"
#include "./lib/gvc/gvc.h"
#include <stdio.h>

int main(int argc, char **argv) {
  Agraph_t *G = agread(stdin, NULL);
  GVC_t *gvc;

  gvLayout(gvc, G, "dot");
  printf("Hello world!\n");
  printf("g = %p\n", G);
  agwrite(G, stdout);
  return 0;
}
