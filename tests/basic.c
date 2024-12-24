#include <stdio.h>
#include <time.h>

int main(void)
{
    const int N = 10000000;

    // These constants are trivially loop-invariant
    int b = 5;
    int c = 7;

    int result = 0;

    clock_t start = clock();

    // Main loop: we do a small mod operation + multiply by the invariant expression
    for (int i = 0; i < N; i++) {
        result += (i % 100) + (b * c);
    }

    clock_t end = clock();

    double elapsed_sec = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Result: %d\n", result);
    printf("Time elapsed: %.3f s\n", elapsed_sec);

    return 0;
}
