#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void matmul(int **A, int **B, int **C, int N)
{
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int sum = 0;
            for (int k = 0; k < N; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

int main()
{
    int N = 512; // Define matrix size
    srand(42);   // Seed for reproducibility

    // Allocate memory for matrices
    int **A = (int **)malloc(N * sizeof(int *));
    int **B = (int **)malloc(N * sizeof(int *));
    int **C = (int **)malloc(N * sizeof(int *));
    for (int i = 0; i < N; i++) {
        A[i] = (int *)malloc(N * sizeof(int));
        B[i] = (int *)malloc(N * sizeof(int));
        C[i] = (int *)malloc(N * sizeof(int));
    }

    // Initialize matrices A and B with random values, and C with zeros
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = rand() % 100; // Random values between 0 and 99
            B[i][j] = rand() % 100;
            C[i][j] = 0;
        }
    }

    // Measure execution time
    clock_t start = clock();
    matmul(A, B, C, N);
    clock_t end = clock();

    // Calculate and display elapsed time
    double elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Matrix multiplication completed in %.3f seconds.\n", elapsed_time);

    return 0;
}
