#include "util.h"

#define OUTER_LOOP(x)                   \
  for (int itr = 0; itr < ITR; itr++) { \
    x                                   \
  }

void original(int aa[LEN][LEN], int bb[LEN][LEN], int cc[LEN][LEN]) {
  OUTER_LOOP(for (int x = 0; x < LEN; x++) { aa[x][x] += bb[x][x] * cc[x][x]; })
}

void rewrite_a(int** aa, int** bb, int** cc) {
  OUTER_LOOP(for (int x = 0; x < LEN; x++) { aa[x][x] += bb[x][x] * cc[x][x]; })
}

void rewrite_b(int* aa, int* bb, int* cc) {
  OUTER_LOOP(for (int x = 0; x < LEN; x++) {
    int idx = (x * LEN) + x;
    aa[idx] += bb[idx] * cc[idx];
  })
}

int main(void) {
#ifdef REWRITE_A
  printf("Doing Rewrite A\n");
  int** aa = (int**)malloc(sizeof(int*) * LEN);
  int** bb = (int**)malloc(sizeof(int*) * LEN);
  int** cc = (int**)malloc(sizeof(int*) * LEN);
  for (int x = 0; x < LEN; x++) {
    aa[x] = (int*)malloc(sizeof(int) * LEN);
    bb[x] = (int*)malloc(sizeof(int) * LEN);
    cc[x] = (int*)malloc(sizeof(int) * LEN);
  }
  init_2d(aa, LEN);
  init_2d(bb, LEN);
  init_2d(cc, LEN);
  rewrite_a(aa, bb, cc);
  uint64_t sum = aggregate_2d(aa, LEN);
  printf("Done: %lu\n", sum);
#elif REWRITE_B
  printf("Doing Rewrite B\n");
  int* aa = (int*)malloc(sizeof(int) * LEN * LEN);
  int* bb = (int*)malloc(sizeof(int) * LEN * LEN);
  int* cc = (int*)malloc(sizeof(int) * LEN * LEN);
  init_1d(aa, LEN);
  init_1d(bb, LEN);
  init_1d(cc, LEN);
  rewrite_b(aa, bb, cc);
  uint64_t sum = aggregate_1d(aa, LEN);
  printf("Done: %lu\n", sum);
#else
  printf("Doing Original\n");
  int aa[LEN][LEN];
  int bb[LEN][LEN];
  int cc[LEN][LEN];
  init_2dd(aa);
  init_2dd(bb);
  init_2dd(cc);
  original(aa, bb, cc);
  uint64_t sum = aggregate_2dd(aa);
  printf("Done: %lu\n", sum);
#endif
}