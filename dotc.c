#include "./lib/cgraph/cgraph.h"
#include "./lib/gvc/gvc.h"
#include <stdio.h>

int main(int argc, char **argv) {
  Agraph_t *G = agread(stdin, NULL);
  GVC_t *gvc = gvContext();

  extern gvplugin_library_t gvplugin_dot_layout_LTX_library;
  gvAddLibrary(gvc, &gvplugin_dot_layout_LTX_library);

  printf("gvLayout\n");
  gvLayout(gvc, G, "dot");

  printf("gvRender\n");
  gvRender(gvc, G, "dot", NULL);

  agwrite(G, stdout);

  return 0;
}
