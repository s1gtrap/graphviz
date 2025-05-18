#include <cgraph.h>
#include <stdio.h>

int main() {
  Agraph_t *g;
  g = agopen("G", Agdirected, NULL);
  printf("Hello, world!\n");
}
