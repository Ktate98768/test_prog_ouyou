#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "matrix.h"

// ----------------------------------------------------------------------------
// 行列演算用関数群
// ----------------------------------------------------------------------------

// mat_alloc: 行列要素用のメモリを確保する
bool mat_alloc(matrix *mat, int rows, int cols) {
    mat->rows = rows;
    mat->cols = cols;
    if(rows <= 0 || cols <= 0){
        return false;
    }
    else{
        mat->elems = (double*)calloc(rows * cols, sizeof(double));
        return true;
    }
}

// mat_free: 使い終わった行列のメモリを解放する
void mat_free(matrix *mat) {
    mat->rows = 0;
    mat->cols = 0;
    free(mat->elems);
    mat->elems = NULL;
}

// mat_print: 行列の中身を表示する
void mat_print(matrix mat) {
    if (mat.rows == 0 || mat.cols == 0 || mat.elems == NULL) {
        fprintf(stderr, "Matrix is NULL or zero size!\n");
        return;
    }

    for (int i = 0; i < mat.rows; i++) {
        for (int j = 0; j < mat.cols; j++) {
            printf("%6.4f%s", mat_elem(mat, i, j), (j == mat.cols - 1) ? "\n" : "  ");
        }
    }
}

// mat_copy: srcの中身を*dstにコピーする
bool mat_copy(matrix *dst, matrix src) {
    if(dst->rows == src.rows && dst->cols == src.cols){
        for(int i = 0; i < dst->rows; i++){
            for(int j = 0; j < dst->cols; j++){
                mat_elem(*dst, i, j) = mat_elem(src, i, j);
            }
        }
        return true;
    }
    else{
        return false;
    }
}

// mat_add: mat1+mat2を*resに代入する
bool mat_add(matrix *res, matrix mat1, matrix mat2) {
    if(mat1.rows == mat2.rows && mat1.cols == mat2.cols && mat1.rows == res->rows && mat1.cols == res->cols){
        for(int i = 0; i < mat1.rows; i++){
            for(int j = 0; j < mat2.cols; j++){
                mat_elem(*res, i, j) = mat_elem(mat1, i, j) + mat_elem(mat2, i, j);
            }
        }
        return true;
    }
    else{
        return false;
    }
}

// mat_sub: mat1-mat2を*resに代入する
bool mat_sub(matrix *res, matrix mat1, matrix mat2) {
    if(mat1.rows == mat2.rows && mat1.cols == mat2.cols && mat1.rows == res->rows && mat1.cols == res->cols){
        for(int i = 0; i < mat1.rows; i++){
            for(int j = 0; j < mat2.cols; j++){
                mat_elem(*res, i, j) = mat_elem(mat1, i, j) - mat_elem(mat2, i, j);
            }
        }
        return true;
    }
    else{
        return false;
    }
}

// mat_mul: mat1とmat2の行列積を*resに代入する
bool mat_mul(matrix *res, matrix mat1, matrix mat2) {
    if(mat1.cols == mat2.rows && mat1.rows == res->rows && mat2.cols == res->cols){
        matrix mat;
        mat_alloc(&mat, res->rows, res->cols);
        double val;
        for(int i = 0; i < res->rows; i++){
            for(int j = 0; j < res->cols; j++){
                val = 0.0;
                for(int k = 0; k < mat1.cols; k++){
                    val += mat_elem(mat1, i, k) * mat_elem(mat2, k, j);
                }
                mat_elem(mat, i, j) = val;
            }
        }
        double *temp = res->elems;
        res->elems = mat.elems;
        mat.elems = temp;
        mat_free(&mat);
        return true;
    }
    else{
        return false;
    }
}

// mat_muls: matをc倍（スカラー倍）した結果を*resに代入する
bool mat_muls(matrix *res, matrix mat, double c) {
    if(mat.rows == res->rows && mat.cols == res->cols){
        for(int i = 0; i < mat.rows; i++){
            for(int j = 0; j < mat.cols; j++){
                mat_elem(*res, i, j) = c * mat_elem(mat, i, j);
            }
        }
        return true;
    }
    else{
        return false;
    }
}

// mat_trans: matの転置行列を*resに代入する
bool mat_trans(matrix *res, matrix mat) {
    if(mat.rows == res->cols && mat.cols == res->rows){
        matrix mat1;
        mat_alloc(&mat1, res->rows, res->cols);
        for(int i = 0; i < mat.rows; i++){
            for(int j = 0; j < mat.cols; j++){
                mat_elem(mat1, j, i) = mat_elem(mat, i, j);
            }
        }
        double *temp = res->elems;
        res->elems = mat1.elems;
        mat1.elems = temp;
        mat_free(&mat1);
        return true;
    }
    else{
        return false;
    }
}

// mat_unit: 単位行列を与える
bool mat_ident(matrix *mat) {
    if(mat->rows == mat->cols){
        for(int i = 0; i < mat->rows * mat->cols; i++){
            mat_elem(*mat, i, i) = 1;
        }
        return true;
    }
    else{
        return false;
    }
}

// mat_equal: mat1とmat2が等しければtrueを返す
bool mat_equal(matrix mat1, matrix mat2) {
int judge = 0;
    if(mat1.rows == mat2.rows && mat1.cols == mat2.cols){
        for(int i = 0; i < mat1.rows; i++){
            for(int j = 0; j < mat1.cols; j++){
                if(fabs(mat_elem(mat1, i , j) - mat_elem(mat2, i , j)) > 1.0e-11){
                    judge += 1;
                }
            }
        }
        if(judge == 0){
            return true;
        }
        else{
            return false;
        }
    }
    else{
        return false;
    }
}

// mat_solve: 連立一次方程式 ax=b を解く．ピボット選択付き
bool mat_solve(matrix *x, matrix A, matrix b) {
    int  i, j, k, l;
    double p;
    matrix A2, b2;

    // 行列のサイズチェック．この例では，bとxを列ベクトルに限定しているが，
    // この部分は後ほど改良する
    if (A.rows != A.cols) return false;
    if ((b.cols != 1) || (A.cols != b.rows)) return false;
    if ((x->rows != b.rows) || (x->cols != 1)) return false;

    // 行列aと行列bの値を書き換えないよう，別の行列を用意する 
    if (!mat_alloc(&A2, A.rows, A.cols)) return false;
    if (!mat_alloc(&b2, b.rows, b.cols)) return false;

    // 用意した行列にAとbの値をコピーする．以下，これらの行列を用いて計算する
    mat_copy(&A2, A);
    mat_copy(&b2, b);

    /*
     * ガウスの消去法：
     * 普通に作れば10行程度． forループを3つ使う？
     * 行列式がゼロかどうかの判定も忘れないこと
     */

    for(i=0; i < A.rows; i++){
        double max = mat_elem(A2, i, i);
        int row = i;
        for(l=i+1; l < A.rows; l++){
            if(fabs(mat_elem(A2, l, i)) > max){
                max = mat_elem(A2, l, i);
                row = l;
            }
        }
        if(fabs(max) < 1.0 * 10e-8){
            mat_free(&A2);
            mat_free(&b2);
            return false;
        }
        else{
            double q;
            for(l=i; l < A.cols; l++){
                q = mat_elem(A2, i, l);
                mat_elem(A2, i, l) = mat_elem(A2, row, l);
                mat_elem(A2, row, l) = q;
            }
            q = mat_elem(b2, i, 0);
            mat_elem(b2, i, 0) = mat_elem(b2, row, 0);
            mat_elem(b2, row, 0) = q;
        }

        for(j=i+1; j < A.rows; j++){
            p = mat_elem(A2, j, i) / mat_elem(A2, i, i);
            for(k=i; k < A.cols; k++){
                mat_elem(A2, j, k) -= mat_elem(A2, i, k) * p;
            }
            mat_elem(b2, j, 0) -= mat_elem(b2, i, 0) * p;
        }
    }

    /*
     *  後退代入：
     *  普通に作れば5-7行程度． forループを2つ使う？
     */
    int Ac = A.cols - 1;
    int br = b.rows - 1;
    for(i=0; i < b.rows; i++){
        for(j=0; j < i; j++){
            mat_elem(b2, br - i, 0) -= mat_elem(A2, br - i, Ac - j) * mat_elem(b2, Ac - j, 0);
        }
        mat_elem(b2, br - i, 0) /= mat_elem(A2, br - i, br - i);
    }

    // 結果を x にコピー
    mat_copy(x, b2);

    // 終わったらメモリを解放
    mat_free(&A2);
    mat_free(&b2);

    return true;
}

// mat_inverse: 行列Aの逆行列を*invAに与える
bool mat_inverse(matrix *invA, matrix A) {
    if (A.rows != A.cols) return false;
    if (invA->rows != A.rows || invA->cols != A.cols) return false;
    matrix A2, b, x;
    if (!mat_alloc(&A2, A.rows, A.cols)) return false;
    if (!mat_alloc(&b, A.rows, 1)) return false;
    if (!mat_alloc(&x, A.rows, 1)) return false;

    for(int i=0; i < A.cols; i++){
        for(int j=0; j < A.rows; j++){
            if (i==j) mat_elem(b, j, 0) = 1;
            else mat_elem(b, j, 0) = 0;
        }
        if(mat_solve(&x, A, b) == false){
            mat_free(&A2);
            mat_free(&b);
            mat_free(&x);
            return false;
        }
        for(int k=0; k < A.rows; k++){
            mat_elem(A2, k, i) = mat_elem(x, k, 0);
        }
    }
    mat_copy(invA, A2);

    mat_free(&A2);
    mat_free(&b);
    mat_free(&x);
    return true;
}
