#include "stdio.h"
#include "stdlib.h"
#include "util.h"

#ifndef LEN
#define LEN 2048
#endif

#ifndef ITR
#define ITR 100
#endif

void original(int aa[LEN][LEN], int bb[LEN][LEN], int cc[LEN][LEN]) {
  for (int itr = 0; itr < ITR; itr++) {
    for (int x = 0; x < LEN; x++) {
      aa[x][x] += bb[x][x] * cc[x][x];
    }
  }
}

void rewrite_a(int** aa, int** bb, int** cc) {
  for (int itr = 0; itr < ITR; itr++) {
    for (int x = 0; x < LEN; x++) {
      aa[x][x] += bb[x][x] * cc[x][x];
    }
  }
}

void rewrite_b(int* aa, int* bb, int* cc) {
  for (int itr = 0; itr < ITR; itr++) {
    for (int x = 0; x < LEN; x++) {
      int idx = (x * LEN) + x;
      aa[idx] += bb[idx] * cc[idx];
    }
  }
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
  int sum = aggregate_2d(aa, LEN);
  printf("Done: %d\n", sum);
#elif REWRITE_B
  printf("Doing Rewrite B\n");
  int* aa = (int*)malloc(sizeof(int) * LEN);
  int* bb = (int*)malloc(sizeof(int) * LEN);
  int* cc = (int*)malloc(sizeof(int) * LEN);
  init_1d(aa, LEN);
  init_1d(bb, LEN);
  init_1d(cc, LEN);
  rewrite_b(aa, bb, cc);
  int sum = aggregate_1d(aa, LEN);
  printf("Done: %d\n", sum);
#else
  printf("Doing Original\n");
  int aa[LEN][LEN];
  int bb[LEN][LEN];
  int cc[LEN][LEN];
  init_2dd(aa);
  init_2dd(bb);
  init_2dd(cc);
  original(aa, bb, cc);
  int sum = aggregate_2dd(aa);
  printf("Done: %d\n", sum);
#endif
}