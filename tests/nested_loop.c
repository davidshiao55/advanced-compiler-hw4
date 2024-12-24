#include <stdio.h>
#include <time.h>

#define N 1000
#define M 1000

int main(int argc, char **argv)
{
    int k = 8;
    int factor = 10;
    int arr[M][N] = {0};

    clock_t start = clock();

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            arr[i][j] = arr[i][j] * factor + (k * i);
        }
    }
    clock_t end = clock();

    double elapsed_sec = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time elapsed: %.3f s\n", elapsed_sec);
    printf("Last Element: %d\n", arr[M - 1][N - 1]);

    return 0;
}