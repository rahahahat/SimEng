#include "util.h"

void init_2d(int** aa, int size) {
  for (int a = 0; a < size; a++) {
    for (int b = 0; b < size; b++) {
      aa[a][b] = 1;
    }
  }
};

void init_2dd(int aa[LEN][LEN]) {
  for (int a = 0; a < LEN; a++) {
    for (int b = 0; b < LEN; b++) {
      aa[a][b] = 1;
    }
  }
}

void init_1d(int* a, int size) {
  for (int b = 0; b < size; b++) {
    a[b] = 1;
  }
}

uint64_t aggregate_2d(int** aa, int size) {
  uint64_t sum = 0;
  for (int a = 0; a < size; a++) {
    for (int b = 0; b < size; b++) {
      sum += aa[a][b];
    }
  }
  return sum;
}
uint64_t aggregate_2dd(int aa[LEN][LEN]) {
  uint64_t sum = 0;
  for (int a = 0; a < LEN; a++) {
    for (int b = 0; b < LEN; b++) {
      sum += aa[a][b];
    }
  }
  return sum;
}
uint64_t aggregate_1d(int* a, int size) {
  uint64_t sum = 0;
  for (int b = 0; b < size; b++) {
    sum += a[b];
  }
  return sum;
}

int** init_swp() {
  int** aa = (int**)malloc(sizeof(int*) * iN);
  for (int x = 0; x < iN; x++) {
    aa[x] = (int*)malloc(sizeof(int) * iN * BLOCKSIZE);
    for (int y = 0; y < iN * BLOCKSIZE; y++) {
      aa[x][y] = 1;
    }
  }
  return aa;
}

uint64_t aggregate_swp(int** a) {
  uint64_t sum = 0;
  for (int x = 0; x < iN; x++) {
    for (int y = 0; y < iN * BLOCKSIZE; y++) {
      sum += a[x][y];
    }
  }
  return sum;
}