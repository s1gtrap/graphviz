#include "lib/gvc/gvc.h"
#include <cgraph.h>
#include <gvc.h>
#include <stdio.h>

int main() {
  GVC_t *gvc;
  Agraph_t *g;

  gvc = gvContext();
  g = agopen("G", Agdirected, NULL);

  gvRender(gvc, g, "dot", stdout);

  printf("Hello, world!\n");
}
