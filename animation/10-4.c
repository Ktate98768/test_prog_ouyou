#ifndef __APPLE__  // OSがMacでない (= WindowsやLinux)
#include <GL/glut.h>  // Windows, Linuxの場合のヘッダ
#else
#include <GLUT/glut.h>  // Macの場合のヘッダ
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "matrix.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define swap(a, b) do { \
    double t = (a);     \
    a = b;              \
    b = t;              \
} while( 0 );

int WINDOW_WIDTH = 500;     // ウィンドウの横幅
int WINDOW_HEIGHT = 500;    // ウィンドウの高さ

// 変形に使う変数
double TIME = 0.0;
double SCALE = 1.0;

// マウスの状態を表す変数
int MOUSE_BUTTON = -1;
int PREV_X = -1;
int PREV_Y = -1;

matrix y_mat0, y_mat1;
int initialized = -1;

// カメラの位置
static const float CAMERA_POS[3] = { 3.0f, 4.0f, 5.0f };

// OpenGLの初期化関数
void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);

    // ライティング機能の有効化
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // 法線の長さが常に1になるようにする
    // glScalefを使う時に特に必要になる
    glEnable(GL_NORMALIZE);

    // シェーディングのやり方
    // GL_FLAT: フラットシェーディング
    // GL_SMOOTH: スムースシェーディング
    glShadeModel(GL_SMOOTH);

    // マテリアルの初期値を設定
    float ambient[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    float diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    float specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    float shininess = 32.0f;
    glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);

    // --- 自分独自の初期化を追加する場合はこの下に記述する ---

    // --- 自分独自の初期化を追加する場合はこの上に記述する ---
}

void f(double c, double m, double g, double k, matrix y, matrix *dydx) {
    matrix temp;
    mat_alloc(&temp,2,1);
    mat_elem(temp,0,0) = mat_elem(y,1,0) - c/m * mat_elem(y,0,0);
    mat_elem(temp,1,0) = -k/m * mat_elem(y,0,0) + g;
    mat_copy(dydx,temp);
    mat_free(&temp);
}

void runge_kutta(double c, double m, double g, double k, matrix y0, double h, matrix *y1, void (*f)(double, double, double, double, matrix, matrix *)) {
    matrix k0, k1, k2, k3, k4;
    mat_alloc(&k0, 2, 1);    
    mat_alloc(&k1, 2, 1);
    mat_alloc(&k2, 2, 1);
    mat_alloc(&k3, 2, 1);
    mat_alloc(&k4, 2, 1);

    f(c, m, g, k, y0, &k1);
    mat_muls(&k2, k1, 0.5);
    mat_muls(&k2, k2, h);
    mat_add(&k2, y0, k2);

    f(c, m, g, k, k2, &k2);
    mat_muls(&k3, k2, 0.5);
    mat_muls(&k3, k3, h);
    mat_add(&k3, k3, y0);

    f(c, m, g, k, k3, &k3);
    mat_muls(&k4, k3, h);
    mat_add(&k4, y0, k4);

    f(c, m, g, k, k4, &k4);
    mat_muls(&k2, k2, 2);
    mat_add(&k0, k1, k2);
    mat_muls(&k3, k3, 2);
    mat_add(&k3, k3, k0);
    mat_add(&k4, k3, k4);
    
    mat_muls(&k4, k4, h/6);
    mat_add(y1, y0, k4);

    mat_free(&k0);
    mat_free(&k1);
    mat_free(&k2);
    mat_free(&k3);
    mat_free(&k4);
}

// 描画関数
void display() {
    // 描画内容のクリア
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // マウスによる変形の設定
    glPushMatrix();
    glScaled(SCALE, SCALE, SCALE);

    // --- この下を変更すると良い ----------------------------
    double k = 20;
    double m = 10;
    double c = 1.0;
    double g = 9.8;

    if (initialized == -1) {
        mat_alloc(&y_mat0, 2, 1);
        mat_alloc(&y_mat1, 2, 1);
        mat_elem(y_mat0, 0, 0) = 0;
        mat_elem(y_mat0, 1, 0) = 0;
        initialized = 0;
    }
    runge_kutta(c, m, g, k, y_mat0, 0.030, &y_mat1, f);
    mat_copy(&y_mat0,y_mat1);

    float ball[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ball);
    glPushMatrix();
    glTranslated(0,mat_elem(y_mat1, 0, 0) - m*g/k,0);
    glutSolidSphere(0.4, 64, 32);
    glPopMatrix();

    // --- この上を変更すると良い ----------------------------

    // マウスによる変形の破棄
    glPopMatrix();

    // 描画命令
    glutSwapBuffers();
}

// ウィンドウサイズ変更時の処理関数
void reshape(int width, int height) {
    // ビューポートの設定
    glViewport(0, 0, width, height);
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;

    // 投影変換行列の設定
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    const double aspect = (double)WINDOW_WIDTH / (double)WINDOW_HEIGHT;
    gluPerspective(45.0, aspect, 0.1, 100.0);

    // モデルビュー行列の設定
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(CAMERA_POS[0], CAMERA_POS[1], CAMERA_POS[2],   // カメラの位置
              0.0, 0.0, 0.0,   // カメラが見ている位置の中心
              0.0, 1.0, 0.0);  // カメラの上方向

    // ライトの位置の設定
    // 好みがなければカメラの位置と同じにすると良い
    float light_position[4] = { CAMERA_POS[0], CAMERA_POS[1], CAMERA_POS[2], 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

// マウスが押された時の処理
void mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        MOUSE_BUTTON = button;
        PREV_X = x;
        PREV_Y = y;
    }

    if (state == GLUT_UP) {
        MOUSE_BUTTON = -1;
        PREV_X = -1;
        PREV_Y = -1;
    }
}

// 線形問題を解く (マウスの処理の補助)
// matrixライブラリをリンクしなくて良いようにしている
bool linsolve(double *A, double *b, int rows, int cols) {
    static const double epsilon = 1.0e-12;

    // ガウスの消去法
    int pivot = 0;
    for (int i = 0; i < rows - 1; i++) {
        // ピボット選択
        pivot = i;
        for (int j = i + 1; j < rows; j++) {
            if (fabs(A[pivot * cols + i]) < fabs(A[j * cols + i])) {
                pivot = j;
            }
        }

        // 行の入れ替え
        for (int j = i; j < cols; j++) {
            swap(A[i * cols + j], A[pivot * cols + j]);
        }
        swap(b[i], b[pivot]);

        // それでもピボットが0ならば特異行列
        if (fabs(A[i * cols + i]) < epsilon) {
            fprintf(stderr, "[ ERROR ] matrix is singular!\n");
            return false;
        }

        // ピボットより下の行を消去する
        for (int j = i + 1; j < rows; j++) {
            const double ratio = A[j * cols + i] / A[i * cols + i];
            for (int k = i; k < cols; k++) {
                A[j * cols + k] -= ratio * A[i * cols + k];
            }
            b[j] -= ratio * b[i];
        }
    }

    // 後退代入
    for (int i = rows - 1; i >= 0; i--) {
        for (int j = i + 1; j < rows; j++) {
            b[i] -= A[i * cols + j] *b[j];
        }
        b[i] /= A[i * cols + i];
    }

    return true;    
}

// マウスが動いた時の処理
// 回転の処理の内容に興味がある人は
// アークボール操作について調べると良い
// https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball
void motion(int x, int y) {
    int dx = x - PREV_X;
    int dy = y - PREV_Y;
    if (dx * dx + dy * dy >= 25.0) {
        if (MOUSE_BUTTON == GLUT_LEFT_BUTTON) {
            double x0 = (double)PREV_X / (double)WINDOW_WIDTH * 2.0 - 1.0;
            double y0 = -1.0 * ((double)PREV_Y / (double)WINDOW_HEIGHT * 2.0 - 1.0);
            double z0 = sqrt(1.0 - min(x0 * x0 + y0 * y0, 1.0));
            double x1 = (double)x / (double)WINDOW_WIDTH * 2.0 - 1.0;
            double y1 = -1.0 * ((double)y / (double)WINDOW_HEIGHT * 2.0 - 1.0);
            double z1 = sqrt(1.0 - min(x1 * x1 + y1 * y1, 1.0));

            double axisX = y0 * z1 - z0 * y1; 
            double axisY = z0 * x1 - x0 * z1; 
            double axisZ = x0 * y1 - y0 * x1;
            double norm = axisX * axisX + axisY * axisY + axisZ * axisZ;
            if (norm > 0.0) {
                norm = sqrt(norm);
                axisX /= norm;
                axisY /= norm;
                axisZ /= norm;
                double angle = 180.0 * acos(max(-1.0, min(x0 * x1 + y0 * y1 + z0 * z1, 1.0))) / M_PI;

                double mv[16];
                glGetDoublev(GL_MODELVIEW_MATRIX, mv);

                double A[9];
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        A[j * 3 + i] = mv[i * 4 + j];
                    }
                }
                double b[3] = { axisX, axisY, axisZ };

                if (linsolve(A, b, 3, 3)) {
                    glMatrixMode(GL_MODELVIEW);
                    glRotated(angle, b[0], b[1], b[2]);
                }
            }
        }

        if (MOUSE_BUTTON == GLUT_RIGHT_BUTTON) {
            SCALE -= 10.0 * dy / WINDOW_HEIGHT;
            SCALE = max(0.1, min(SCALE, 10.0));
        }

        PREV_X = x;
        PREV_Y = y;
    }
}

// キーボードが押された時の処理
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case '\e':  // Escキー
            mat_free(&y_mat0);
            mat_free(&y_mat1);
            exit(1);
            break;

        default:
            break;
    }
}

// アニメーションの処理
void timer(int value) {
    TIME += 0.030;
    glutPostRedisplay();
    glutTimerFunc(30, timer, 0);
}

// メイン関数
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("GLUT 3D graphics");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(30, timer, 0);

    init();

    glutMainLoop();
}
