#ifndef __APPLE__  // OSがMacでない (= WindowsやLinux)
#include <GL/glut.h>  // Windows, Linuxの場合のヘッダ
#else
#include <GLUT/glut.h>  // Macの場合のヘッダ
#endif

#include <stdio.h>
#include "matrix.h"

#define MAX_POINTS 5

int WINDOW_WIDTH = 500;   // ウィンドウの横幅
int WINDOW_HEIGHT = 500;  // ウィンドウの高さ

int n_points;
matrix points;

void lagrange(double t, matrix points, double *x, double *y) {
    matrix L;
    double p;
    mat_alloc(&L, points.rows, points.cols);
    //分子
    double num = 1;
    //分母
    double den = 1;
    
    for(int u=0; u<points.cols; u++){
        for(int i=0; i<points.rows; i++){
            num = 1;
            den = 1;
            for(int j=0; j<points.rows; j++){
                if(i != j){
                    den *= i - j;
                    num *= t - j;
                }
            }
            mat_elem(L, i, u) = num / den;
        }
    }
    
    for(int u=0; u<points.cols; u++){
        p = 0;
        for(int i=0; i<points.rows; i++){
            p += mat_elem(L, i, u) * mat_elem(points, i, u);
        }
        if(u==0){
            *x = p;
        } else {
            *y = p;
        }
    }
    
    mat_free(&L);
}

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // 頂点情報の初期化
    n_points = 0;
    mat_alloc(&points, MAX_POINTS, 2);
}

void display() {
    // ウィンドウ表示内容のクリア
    glClear(GL_COLOR_BUFFER_BIT);

    // 点をあるだけ描く
    glPointSize(10.0);
    glBegin(GL_POINTS);
    for (int i = 0; i < n_points; i++) {
        const double x_p = mat_elem(points, i, 0);
        const double y_p = mat_elem(points, i, 1);
        glVertex2d(x_p, y_p);
    }
    glEnd();

    // 曲線を描く
    const double segments = 100;
    double x, y;
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= segments; i++) {
        // 以下のtは0から(n_points - 1)までをsegment分割したもの(この場合は100分割)
        const double t = (double)((n_points - 1) * i) / (double)segments;
        lagrange(t, points, &x, &y);
        glVertex2d(x, y);
    }
    glEnd();

    // 表示内容の更新
    glFlush();
}

void reshape(int width, int height) {
    // OpenGLウィンドウの描画範囲を設定
    // 下記は描画範囲が[0, width] x [0, height]となるように設定している
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (double)width, 0.0, (double)height);
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (n_points < MAX_POINTS) {
            mat_elem(points, n_points, 0) = x;
            mat_elem(points, n_points, 1) = WINDOW_HEIGHT - y - 1;  // Y座標は上下反転する
            n_points++;
        } else {
            printf("点の数は%dを超えられません\n", MAX_POINTS);
        }
        glutPostRedisplay();
    }
}

int main(int argc, char **argv) {
    // GLUTの初期化
    glutInit(&argc, argv);
    // ウィンドウのサイズを設定
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    // ウィンドウの作成 (引数はウィンドウのタイトル)
    glutCreateWindow("First GLUT program");
    // 描画に使う関数の登録
    glutDisplayFunc(display);
    // ウィンドウのサイズ変更時に呼ばれる関数の登録
    glutReshapeFunc(reshape);
    // マウス操作時に呼ばれる関数の登録
    glutMouseFunc(mouse);
    // OpenGLの初期化処理 (これはコールバック関数ではないので直接呼び出す)
    init();
    // 描画ループの開始
    glutMainLoop();
}
