#include <stdio.h>
#include <math.h>
#include <omp.h>
#include "matrix.h"

// 乱数生成器
#define RAND_MAX_ 32767

static unsigned long int seed_ = 1;
void srand_(unsigned long int seed) {
    seed_ = seed;
}

int rand_() {
    seed_ = seed_ * 1103515245 + 12345;
    return (unsigned int)(seed_ / 65536) % 32768;
}

// スレッド数
int get_num_threads() {
    int threads = 1;
    #pragma omp parallel
    {
        #pragma omp single
        {
            threads = omp_get_num_threads();
        }
    }
    return threads;
}

// べき乗法
// Aは固有値・固有ベクトルを求める行列
// xは関数の内部で変更して固有ベクトルになる
// 戻り値が固有値
double power_method(matrix A, matrix *x) {
    const int N = 10000;
    // xをランダムベクトルで初期化
    for (int i = 0; i < x->rows; i++) {
        mat_elem(*x, i, 0) = rand_() / (double)RAND_MAX_;
    }

    double x_norm;
    #pragma omp parallel for reduction(+:x_norm)
    for(int i=0; i<N; i++) {
        mat_mul(&x, A, x);
        x_norm = 0;
        for(int t=0; t<x->rows; t++) {
            x_norm += pow(mat_elem(x, t, 0), 2);
        }
        x_norm = sqrt(x_norm);
        mat_muls(&x, x, 1 / x_norm);
    }
    
    matrix x_trans, lambda;
    mat_alloc(x_trans, x->cols, x->rows);
    mat_alloc(&lambda, 1, 1);
    mat_trans(&x_trans, x);
    mat_mul(&x_trans, x_trans, A);
    mat_mul(&lambda_mat, x_trans, x);
    return mat_elem(lambda_mat, 0, 0);
}

// 逆べき乗法
// Aは固有値・固有ベクトルを求める行列
// xは関数の内部で変更して固有ベクトルになる
// 戻り値が固有値
double inv_power_method(matrix A, matrix *x) {
    const int N = 10000;
    // xをランダムベクトルで初期化
    for (int i = 0; i < x->rows; i++) {
        mat_elem(*x, i, 0) = rand_() / (double)RAND_MAX_;
    }

    return 0.0;
}

void test_simple() {
    // 単純な行列でのテスト
    printf("***** Small test *****\n");
    matrix A;
    mat_alloc(&A, 2, 2);
    mat_elem(A, 0, 0) = 1.0;
    mat_elem(A, 1, 0) = 3.0;
    mat_elem(A, 0, 1) = 3.0;
    mat_elem(A, 1, 1) = 1.0;

    matrix x0, x1;
    mat_alloc(&x0, 2, 1);
    mat_alloc(&x1, 2, 1);

    // べき乗法・逆べき乗法
    double l0 = power_method(A, &x0);
    double l1 = inv_power_method(A, &x1);

    printf("Eigenvalues: %f, %f\n", l0, l1);
    printf("u0:\n");
    mat_print(x0);
    printf("u1:\n");
    mat_print(x1);

    // メモリ解放
    mat_free(&A);
    mat_free(&x0);
    mat_free(&x1);
}

void test_large() {
    // 大きな行列でテスト
    printf("***** Large test *****\n");
    const int N = 500;

    matrix A;
    mat_alloc(&A, N, N);
    for (int i = 0; i < N; i++) {
        for (int j = i; j < N; j++) {
            double val = rand_() / (double)RAND_MAX_;
            mat_elem(A, i, j) = val;
            mat_elem(A, j, i) = val;
        }
    }

    matrix x0, x1;
    mat_alloc(&x0, N, 1);
    mat_alloc(&x1, N, 1);

    // べき乗法・逆べき乗法
    double start, end;

    start = omp_get_wtime();
    double l0 = power_method(A, &x0);
    end = omp_get_wtime();
    printf("Time (power): %f ms\n", 1000.0 * (end - start));

    start = omp_get_wtime();
    double l1 = inv_power_method(A, &x1);
    end = omp_get_wtime();
    printf("Time (invpow): %f ms\n", 1000.0 * (end - start));

    printf("Eigenvalues: %f, %f\n", l0, l1);

    // メモリ解放
    mat_free(&A);
    mat_free(&x0);
    mat_free(&x1);
}

int main(int argc, char **argv) {
    // シードを固定
    srand_(31415);

    // スレッド数のチェック
    printf("#thread: %d\n", get_num_threads());

    // テストの実行
    test_simple();
    test_large();
}
