#include "util.h"

void original(int** ppdHJMPath, int in, int blocksize) {
  int i, j;
  for (int itr = 0; itr < ITR; itr++) {
    for (int b = 0; b < blocksize; b++) {
      for (j = 0; j < in; j++) {
        int idx = blocksize * j + b;
        for (i = 1; i < in; ++i) {
          ppdHJMPath[i][idx] = 0;
        }
      }
    }
  }
}

void rewrite(int** ppdHJMPath, int in, int blocksize) {
  int i, j;
  for (int itr = 0; itr < ITR; itr++) {
    for (int b = 0; b < blocksize; b++) {
      for (j = 1; j < in; j++) {
        for (i = 0; i < in; ++i) {
          int idx = blocksize * i + b;
          ppdHJMPath[j][idx] = 0;
        }
      }
    }
  }
}

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