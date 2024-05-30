#include "swp.h"
#include "util.h"

int main(void) {
  int** a = init_swp();
#ifdef REWRITE
  printf("Doing Rewrite\n");
  rewrite(a, iN, BLOCKSIZE);
  uint64_t sum = aggregate_swp(a);
  printf("Done: %lu\n", sum);
#else
  printf("Doing Original\n");
  original(a, iN, BLOCKSIZE);
  uint64_t sum = aggregate_swp(a);
  printf("Done: %lu\n", sum);
#endif
}