#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

double calc_pi_simple(int n) {
    double sum = 0.0;
    for (int i = 1; i <= n; i++) {
        sum += (1.0 / i) / i;
    }
    return sqrt(sum * 6.0);
}

double calc_pi_lock(int n) {
    omp_lock_t lock;
    omp_init_lock(&lock);
    double sum = 0.0;
    #pragma omp parallel for
    for (int i = 1; i <= n; i++) {
        omp_set_lock(&lock);
        sum += (1.0 / i) / i;
        omp_unset_lock(&lock);
    }
    return sqrt(sum * 6.0);
}

double calc_pi_critical(int n) {
    double sum = 0.0;
    #pragma omp parallel for
    for (int i = 1; i <= n; i++) {
        #pragma omp critical
        {
            sum += (1.0 / i) / i;
        }
    }
    return sqrt(sum * 6.0);
}

double calc_pi_atomic(int n) {
    double sum = 0.0;
    #pragma omp parallel for
    for (int i = 1; i <= n; i++) {
        #pragma omp atomic
        sum += (1.0 / i) / i;
    }
    return sqrt(sum * 6.0);
}

int main(int argc, char **argv) {
    // 項数の初期値は1000とし，適宜コマンドライン引数から項数を読み取る
    int N = 1000;
    if (argc > 1) N = atoi(argv[1]);

    double pi, start, end;

    // 単純計算
    start = omp_get_wtime();
    pi = calc_pi_simple(N);
    end = omp_get_wtime();
    printf("Pi (w/o parallel): %.12f\n", pi);
    printf("Time: %f ms\n", 1000.0 * (end - start));

    //ロック
    start = omp_get_wtime();
    pi = calc_pi_lock(N);
    end = omp_get_wtime();
    printf("Pi (lock): %.12f\n", pi);
    printf("Time: %f ms\n", 1000.0 * (end - start));

    //クリティカル
    start = omp_get_wtime();
    pi = calc_pi_critical(N);
    end = omp_get_wtime();
    printf("Pi (critical): %.12f\n", pi);
    printf("Time: %f ms\n", 1000.0 * (end - start));

    //アトミック
    start = omp_get_wtime();
    pi = calc_pi_atomic(N);
    end = omp_get_wtime();
    printf("Pi (atomic): %.12f\n", pi);
    printf("Time: %f ms\n", 1000.0 * (end - start));
}