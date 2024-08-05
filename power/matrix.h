#ifndef _MATRIX_H
#define _MATRIX_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

// 要素を交換するマクロ
#define swap(a, b) do { \
    double t = (a);     \
    a = b;              \
    b = t;              \
} while( 0 );

/*
 * 行列用構造体
 * rows: 行数
 * cols: 列数
 * elems: 行列要素を入れた一次元配列
 */
typedef struct {
    int rows;
    int cols;
    double *elems;
} matrix;

// 行列要素を取得するマクロ
//mは行列、iは(0から数えて)何行目か、jは(0から数えて)何列目か
#define mat_elem(m, i, j) (m).elems[(i) * (m).cols + (j)]

// 行列のメモリ確保 (成否に合わせてブール値を返す)
bool mat_alloc(matrix *mat, int row, int col);
// 行列のメモリ開放
void mat_free(matrix *mat);
// 行列のコピー (サイズが異なるなどコピー不可ならfalseを返す)
bool mat_copy(matrix *dst, matrix src);
// 行列の加算 (サイズが異なるなど計算が不可ならfalseを返す)
bool mat_add(matrix *dst, matrix mat1, matrix mat2);
// 行列の減算 (サイズが異なるなど計算が不可ならfalseを返す)
bool mat_sub(matrix *dst, matrix mat1, matrix mat2);
// 行列の積 (サイズが不適合など計算が不可ならfalseを返す)
bool mat_mul(matrix *dst, matrix mat1, matrix mat2);
// 行列のスカラ倍 (サイズが異なるなど計算が不可ならfalseを返す)
bool mat_muls(matrix *dst, matrix mat, double s);
// 行列の転置 (サイズが不適合ならfalseを返す)
bool mat_trans(matrix *dst, matrix src);
// 行列を単位行列にする (行列が正方でなければfalseを返す)
bool mat_ident(matrix *mat);
// 二つの行列mat1とmat2が等しいかを判定
bool mat_equal(matrix mat1, matrix mat2);
// 行列の中身を標準出力する
void mat_print(matrix mat);
// mat_solve: 連立一次方程式 ax=b を解く．ピボット選択付き
bool mat_solve(matrix *x, matrix A, matrix b);
// mat_inverse: 行列Aの逆行列を*invAに与える
bool mat_inverse(matrix *invA, matrix A);

#endif