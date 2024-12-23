#include <stdio.h>
#include <time.h>

// A simple program to test custom LICM + performance timing
int main(void) {
    const int N = 10000000;      // Number of iterations
    static int arr[100000];      // Array large enough to create some overhead
    int c1 = 5;                  // Invariant constant
    int c2 = 10;                 // Another invariant constant

    // Initialize arr with some pattern
    for (int i = 0; i < 100000; i++) {
        arr[i] = i % 100 + 1;    // E.g. 1..100 repeated
    }

    // Simple micro-benchmark: sum up arr[i] * c1 * c2
    clock_t start = clock();

    long long sum = 0; 
    for (int i = 0; i < N; i++) {
        // The product c1*c2 is loop-invariant
        sum += arr[i % 100000] * c1 * c2;
    }

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Result: %lld\n", sum);
    printf("Time elapsed: %.4f s\n", elapsed);
    return 0;
}
