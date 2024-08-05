#ifndef __APPLE__  // OSがMacでない (= WindowsやLinux)
#include <GL/glut.h>  // Windows, Linuxの場合のヘッダ
#else
#include <GLUT/glut.h>  // Macの場合のヘッダ
#endif

#include <stdio.h>
#include <math.h>

int WINDOW_WIDTH = 500;   // ウィンドウの横幅
int WINDOW_HEIGHT = 500;  // ウィンドウの高さ

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
}

void disp_T(double r, double g, double b) {
    glColor3d(r,g,b);
    glRectd(20, 450, 200, 420);
    glRectd(95, 450, 125, 270);
}

void disp_K(double r, double g, double b) {
    glColor3d(r,g,b);
    glRectd(300, 220, 330, 50);

    glLineWidth(50.0);
    glBegin(GL_LINE_STRIP);
    glVertex2d(420, 210);
    glVertex2d(315, 135);
    glVertex2d(420, 60);
    glEnd();
}

void disp_circle(double r, double g, double b, double x, double y, double radius) {
    double theta = 0;
    glColor3d(r,g,b);
    
    glLineWidth(5.0);
    glBegin(GL_POLYGON);
    for(int i=0; i<1000; i++) {
        glVertex2d(x + radius * cos(theta), y + radius * sin(theta));
        theta += 2 * M_PI / 1000;
    }
    glEnd();
}

void disp_reg_polygon(double r, double g, double b, double x, double y, double num, double den, double radius) {
    double theta = M_PI / 2;
    glColor3d(r,g,b);
    
    glLineWidth(1.0);
    glBegin(GL_LINE_STRIP);
    for(int i=0; i<= num; i++) {
        glVertex2d(x + radius * cos(theta), y + radius * sin(theta));
        theta += 2 * M_PI / (num/den);
    }
    glEnd();
}

void display() {
    // ウィンドウ表示内容のクリア
    glClear(GL_COLOR_BUFFER_BIT);

    //Tの文字
    disp_T(1,0,0);
    //Kの文字
    disp_K(0,1,0);
    //円
    disp_circle(0,0,1,350,350,100);
    //正n角形(nは有理数)
    disp_reg_polygon(0.1,0.7,0.4,120,120,13,7,80);

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
    // OpenGLの初期化処理  (これはコールバック関数ではないので直接呼び出す)
    init();
    // 描画ループの開始
    glutMainLoop();
}