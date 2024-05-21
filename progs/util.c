#include "util.h"

void init_2d(int **aa, int size) {
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

void init_1d(int *a, int size) {
    for (int b = 0; b < size * size; b++) {
        a[b] = 1;
    }
}

int aggregate_2d(int**aa, int size) {
    int sum = 0;
    for (int a = 0; a < size; a++) {
        for (int b = 0; b < size; b++) {
            sum += aa[a][b];
        }
    }
    return sum;
}
int aggregate_2dd(int aa[LEN][LEN]) {
    int sum = 0;
    for (int a = 0; a < LEN; a++) {
        for (int b = 0; b < LEN; b++) {
           sum +=  aa[a][b] = 1;
        }
    }
    return sum;
}
int aggregate_1d(int*a, int size) {
    int sum = 0;
    for (int b = 0; b < size * size; b++) {
        sum += a[b];
    }
    return sum;
}