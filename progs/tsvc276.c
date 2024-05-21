#include "util.h"

void original(int* a, int* b, int* c, int* d) {
  int mid = (LEN * LEN / 2);
  for (int nl = 0; nl < ITR; nl++) {
    for (int i = 0; i < LEN * LEN; i++) {
      if (i < mid) {
        a[i] += b[i] * c[i];
      } else {
        a[i] += b[i] * d[i];
      }
    }
  }
}

void rewrite(int* a, int* b, int* c, int* d) {
  int mid = (LEN * LEN / 2);
  for (int nl = 0; nl < ITR; nl++) {
    for (int i = 0; i < mid; i++) {
      a[i] += b[i] * c[i];
    }
    for (int i = mid; i < LEN * LEN; i++) {
      a[i] += b[i] * d[i];
    }
  }
}

int main(void) {
  int* aa = (int*)malloc(sizeof(int) * LEN * LEN);
  int* bb = (int*)malloc(sizeof(int) * LEN * LEN);
  int* cc = (int*)malloc(sizeof(int) * LEN * LEN);
  int* dd = (int*)malloc(sizeof(int) * LEN * LEN);
  init_1d(aa, LEN);
  init_1d(bb, LEN);
  init_1d(cc, LEN);
  init_1d(dd, LEN);
#ifdef REWRITE
  printf("Doing Rewrite\n");
  rewrite(aa, bb, cc, dd);
  uint64_t sum = aggregate_1d(aa, LEN);
  printf("Done: %lu\n", sum);
#else
  printf("Doing Original\n");
  original(aa, bb, cc, dd);
  uint64_t sum = aggregate_1d(aa, LEN);
  printf("Done: %lu\n", sum);
#endif
}