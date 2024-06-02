#ifndef _UTIL_
#define _UTIL_

#ifndef LEN
#define LEN 512
#endif

#ifndef ITR
#define ITR 200
#endif

#ifndef BLOCKSIZE
#define BLOCKSIZE 64
#endif

#ifndef iN
#define iN 10
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void init_2d(int** aa, int size);
void init_2dd(int aa[LEN][LEN]);
void init_1d(int* a, int size);

uint64_t aggregate_2d(int** aa, int size);
uint64_t aggregate_2dd(int a[LEN][LEN]);
uint64_t aggregate_1d(int* a, int size);

int** init_swp();
uint64_t aggregate_swp(int** a);

#endif