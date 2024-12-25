#include "./lib/cgraph/cgraph.h"
#include "./lib/gvc/gvc.h"
#include <stdio.h>

int main(int argc, char **argv) {
  GVC_t *gvc = gvContext();

  Agraph_t *G = agread(stdin, NULL);
  // Agraph_t *G = agmemread("digraph { A -> B }");

  extern gvplugin_library_t gvplugin_core_LTX_library;
  gvAddLibrary(gvc, &gvplugin_core_LTX_library);

  extern gvplugin_library_t gvplugin_dot_layout_LTX_library;
  gvAddLibrary(gvc, &gvplugin_dot_layout_LTX_library);

  gvLayout(gvc, G, "dot");

  gvRender(gvc, G, "svg", NULL);

  return 0;
}
