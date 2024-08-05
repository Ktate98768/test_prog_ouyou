#ifndef __APPLE__  // OSがMacでない (= WindowsやLinux)
#include <GL/glut.h>  // Windows, Linuxの場合のヘッダ
#else
#include <GLUT/glut.h>  // Macの場合のヘッダ
#endif

#include <stdio.h>
#include <math.h>

int WINDOW_WIDTH = 500;   // ウィンドウの横幅
int WINDOW_HEIGHT = 500;  // ウィンドウの高さ

double TIME = 0.0;
const double RECT_SIZE = 300.0;

int PREV_X;
int PREV_Y;
int PRESS_BUTTON = -1;
int pressed = -1;
double r = 0;
double g = 1;

void display() {
    // ウィンドウ表示内容のクリア
    glClear(GL_COLOR_BUFFER_BIT);

    if (pressed != -1) {
        glColor3d(r, g, 0);

        glPointSize(20.0);
        glBegin(GL_POINTS);
        glVertex2d(PREV_X, WINDOW_HEIGHT - PREV_Y - 1);
        glEnd();
    }
    
    // バッファの入れ替え
    glutSwapBuffers();
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
    // 再描画
    glutPostRedisplay();

    // 100ミリ秒後にtimer関数を引数0で自分自身を呼び出す
    glutTimerFunc(10, timer, 0);
}

void mouse(int button, int state, int x, int y) {
    // ボタンが押されたら押されたボタンを記憶しておく
    if (state == GLUT_DOWN) {
        pressed = 0;
        PRESS_BUTTON = button;
        PREV_X = x;
        PREV_Y = y;
    }

    // ボタンが離されたらボタンの記憶をクリアする
    if (state == GLUT_UP) {
        PRESS_BUTTON = -1;
    }

    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if(r == 1) {
            r = 0;
            g = 1;
        } else {
            r = 1;
            g = 0;
        }
    }
}

void motion(int x, int y) {
    // あまり頻繁に描画内容が更新されないように前の位置からの
    // 距離が一定以上になっているかを判定する
    int dx = PREV_X - x;
    int dy = PREV_Y - y;
    if (dx * dx + dy * dy > 25.0) {
        if (PRESS_BUTTON == GLUT_LEFT_BUTTON) {
            PREV_X = x;
            PREV_Y = y;
        }
    }
}

int main(int argc, char **argv) {
    // GLUTの初期化
    glutInit(&argc, argv);
    // GLUTの機能の有効化
    glutInitDisplayMode(GLUT_DOUBLE);
    // ウィンドウのサイズを設定
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    // ウィンドウの作成 (引数はウィンドウのタイトル)
    glutCreateWindow("GLUT: Timer");
    // 描画に使う関数の登録
    glutDisplayFunc(display);
    // ウィンドウのサイズ変更時に呼ばれる関数の登録
    glutReshapeFunc(reshape);
    // 100ミリ秒後にtimer関数を引数0で呼び出す
    glutTimerFunc(10, timer, 0);
    // マウス関係のコールバック関数
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    // 描画ループの開始
    glutMainLoop();
}