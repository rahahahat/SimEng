#ifndef _UTIL_
#define _UTIL_

#ifndef LEN
#define LEN 2048
#endif

void init_2d(int **aa, int size);
void init_2dd(int aa[LEN][LEN]);
void init_1d(int *a, int size);

int aggregate_2d(int**aa, int size);
int aggregate_2dd(int a[LEN][LEN]);
int aggregate_1d(int*a, int size);

#endif