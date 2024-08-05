#ifndef __APPLE__  // OSがMacでない (= WindowsやLinux)
#include <GL/glut.h>  // Windows, Linuxの場合のヘッダ
#else
#include <GLUT/glut.h>  // Macの場合のヘッダ
#endif

#include <math.h>

int WINDOW_WIDTH = 500;   // ウィンドウの横幅
int WINDOW_HEIGHT = 500;  // ウィンドウの高さ

double TIME = 0.0;
const double RECT_SIZE = 300.0;

void display() {
    // ウィンドウ表示内容のクリア
    glClear(GL_COLOR_BUFFER_BIT);

    // 正方形の4つ角の位置を計算
    double scale = (cos(TIME / 18.0) + 1.0) * 0.5;
    double x0 = WINDOW_WIDTH / 2 - RECT_SIZE / 2 * scale;
    double y0 = WINDOW_HEIGHT / 2 - RECT_SIZE / 2 * scale;
    double x1 = WINDOW_WIDTH / 2 + RECT_SIZE / 2 * scale;
    double y1 = WINDOW_HEIGHT / 2 + RECT_SIZE / 2 * scale;
    
    // 正方形の描画
    glRectd(x0, y0, x1, y1);
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

void timer(int value) {
    // アニメーションに使う時間を更新
    TIME += 1.0;

    // 再描画
    glutPostRedisplay();

    // 100ミリ秒後にtimer関数を引数0で自分自身を呼び出す
    glutTimerFunc(100, timer, 0);
}

int main(int argc, char **argv) {
    // GLUTの初期化
    glutInit(&argc, argv);
    // ウィンドウのサイズを設定
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    // ウィンドウの作成 (引数はウィンドウのタイトル)
    glutCreateWindow("GLUT: Timer");
    // 描画に使う関数の登録
    glutDisplayFunc(display);
    // ウィンドウのサイズ変更時に呼ばれる関数の登録
    glutReshapeFunc(reshape);
    // 100ミリ秒後にtimer関数を引数0で呼び出す
    glutTimerFunc(100, timer, 0);
    // 描画ループの開始
    glutMainLoop();
}