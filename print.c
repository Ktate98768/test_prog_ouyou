#include <stdio.h>

#ifdef _OPENMP
#include <omp.h>
#endif

int main() {
    #pragma omp parallel
    {
        #pragma omp single
        {
            printf("#threads: %d\n", omp_get_num_threads());
        }

        printf("Hello, world!\n");
    }

    #pragma omp parallel for
    for (int i = 0; i < 10; i++) {
        printf("%d\n", i);
    }
}