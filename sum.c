#include <stdio.h>

#ifdef _OPENMP
#include <omp.h>
#endif

int main() {
    int sum = 0;
    int sum_open = 0;
    #pragma omp parallel for
    for (int i = 1; i <= 10000; i++) {
        #pragma omp critical
        {
            sum_open += i;
        }
    }
    printf("result:%d\n",sum_open);
    printf("right answer:50005000\n");
}