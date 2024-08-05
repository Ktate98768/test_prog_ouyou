#ifndef __APPLE__  // OSがMacでない (= WindowsやLinux)
#include <GL/glut.h>  // Windows, Linuxの場合のヘッダ
#else
#include <GLUT/glut.h>  // Macの場合のヘッダ
#endif

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "matrix.h"
#include "linked_list.h"

int WINDOW_WIDTH = 800;   // ウィンドウの横幅
int WINDOW_HEIGHT = 750;  // ウィンドウの高さ

double TIME = 0.0;
unsigned int TIME_INV = 0;
const double RECT_SIZE = 300.0;

int PREV_X;
int PREV_Y;

int beam_spead = 0;
int invader_rows;
int speed;

const double N = 1024;
const double h = 0.01;
double k = 0.001;
double m = 1000;
double g = 98 * 3;

int game = 0;
int x_move = 1;

linked_list list;
linked_list list_debris_pos;
linked_list list_debris_dif;

typedef struct {
    int x, y;
    bool state;
    int dead_judge;
} invader;

invader *inv;

#define alive true
#define dead false

void f(double g, double x, matrix y, matrix *dydx) {
    matrix temp;
    mat_alloc(&temp,2,1);
    mat_elem(temp,0,0) = -mat_elem(y,1,0) + x * mat_elem(y,0,0);
    mat_elem(temp,1,0) = g;
    mat_copy(dydx,temp);
    mat_free(&temp);
}

void runge_kutta(double g, double x, matrix y0, double h, matrix *y1, void (*f)(double, double, matrix, matrix *)) {
    matrix k0, k1, k2, k3, k4;
    mat_alloc(&k0, 2, 1);    
    mat_alloc(&k1, 2, 1);
    mat_alloc(&k2, 2, 1);
    mat_alloc(&k3, 2, 1);
    mat_alloc(&k4, 2, 1);

    f(g, x, y0, &k1);
    mat_muls(&k2, k1, 0.5);
    mat_muls(&k2, k2, h);
    mat_add(&k2, y0, k2);

    f(g, x, k2, &k2);
    mat_muls(&k3, k2, 0.5);
    mat_muls(&k3, k3, h);
    mat_add(&k3, k3, y0);

    f(g, x, k3, &k3);
    mat_muls(&k4, k3, h);
    mat_add(&k4, y0, k4);

    f(g, x, k4, &k4);
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

void explode_invader(invader *inv) {
    if(inv->state == dead && inv->dead_judge == 0) {
        inv->dead_judge = 1;
        int r;
        int r_x, r_y;
        srand((unsigned int)TIME_INV);
        r = rand() % 6 + 5;
        for(int i=0; i<r; i++){
            if(list_debris_pos.root == NULL) {
                list_node *node = NULL;
                node->x = inv->x * 1e6;
                node->y = (WINDOW_HEIGHT - inv->y - 1) * 1e6;
                list_debris_pos.root = node;
            } else {
                add_node(&list_debris_pos, inv->x * 1e6, (WINDOW_HEIGHT - inv->y - 1) * 1e6);
            }
            r_x = rand() % 2000001;
            r_y = rand() % 200000001;
            if(list_debris_dif.root == NULL) {
                list_node *node = NULL;
                node->x = r_x - 1000000;
                node->y = r_y - 100000000;
                list_debris_dif.root = node;
            } else {
                add_node(&list_debris_dif, r_x - 1000000, r_y - 100000000);
            }
        }
    }
}

void move_debris(list_node *node_pos, list_node *node_dif) {
    matrix y0, y1;
    mat_alloc(&y0, 2, 1);
    mat_alloc(&y1, 2, 1);
    mat_elem(y0, 0, 0) = (double)(node_pos->y) * 1e-6;
    mat_elem(y0, 1, 0) = (double)(node_dif->y) * 1e-6;

    runge_kutta(g, k/m, y0, h, &y1, f);
    node_pos->y = mat_elem(y1, 0, 0) * 1e6;
    node_dif->y = mat_elem(y1, 1, 0) * 1e6;
    node_pos->x += node_dif->x;
    mat_free(&y0);
    mat_free(&y1);
}

int game_difficulty_speed(int difficulty) {
    if(difficulty == 0) {
        return 1;
    } else if(difficulty == 1) {
        return 2;
    } else if(difficulty == 2) {
        return 4;
    } else {
        return 0;
    }
}
int game_difficulty_invader_rows(int difficulty) {
    if(difficulty == 0) {
        return 4;
    } else if(difficulty == 1) {
        return 8;
    } else if(difficulty == 2) {
        return 16;
    } else {
        return 1;
    }
}

int gameover(invader *inv) {
    if (inv->y > 600 && inv->state == alive) {
        return 1;
    }
    return 0;
}

void init_invader(int x, int y, invader *inv) {
    inv->x = x;
    inv->y = y;
    inv->dead_judge = 0;
    inv->state = alive;
}

void summon_invader(int size, invader *inv) {
    if (inv->state == alive) {
        int invader[8][11] = {  
            {0,0,1,0,0,0,0,0,1,0,0},
            {0,0,0,1,0,0,0,1,0,0,0},
            {0,0,1,1,1,1,1,1,1,0,0},
            {0,1,1,0,1,1,1,0,1,1,0},
            {1,1,1,1,1,1,1,1,1,1,1},
            {1,0,1,1,1,1,1,1,1,0,1},
            {1,0,1,0,0,0,0,0,1,0,1},
            {0,0,0,1,1,0,1,1,0,0,0}
        };
        int dx;
        int dy;
        for(int j=0; j<11; j++) {
            for(int i=0; i<8; i++) {
                if(invader[i][j] == 1){
                    dx = (j - 5) * size;
                    dy = (i - 4) * size;
                    glPointSize(size);
                    glBegin(GL_POINTS);
                    glVertex2d(inv->x + dx, WINDOW_HEIGHT - (inv->y + dy) - 1);
                    glEnd();
                }
            }
        }
    }
}

void move_invader(int speed, invader *inv) {
    if((int)(TIME * 100) % 10 == 0) {
        inv->y += speed;
    }

    if(inv->x > WINDOW_WIDTH - 50) {
        x_move = -1;
    }
    else if (inv->x < 50) {
        x_move = 1;
    }
    inv->x += x_move;
}

void kill_invader(linked_list list, invader *inv, int size) {
    //atari_jugdge_debug(inv, size);
    if(list.root != NULL) {
        list_node *kill = list.root;
        double dx = kill->x - inv->x;
        double dy = kill->y - (WINDOW_HEIGHT - inv->y - 1);
        while(kill != NULL) {
            dx = kill->x - inv->x;
            dy = kill->y - (WINDOW_HEIGHT - inv->y - 1);
            if(fabs(dx) <= size * 5 && fabs(dy) <= 20 && inv->state == alive) {
                inv->state = dead;
                kill->x = -300;
            }
            kill = kill->next;
        }
    }
}

void display() {
    // ウィンドウ表示内容のクリア
    glClear(GL_COLOR_BUFFER_BIT);
    if(game != 0) glClearColor(0.4, 0.0, 0.0, 1.0);
    
    glLineWidth(2.0);
    glColor3d(0.5, 0.5, 0.5);
    glBegin(GL_LINES);
    glVertex2d(0,WINDOW_HEIGHT - 600 - 1);
    glVertex2d(WINDOW_WIDTH,WINDOW_HEIGHT - 600 - 1);
    glEnd();

    glColor3d(1.0, 1.0, 1.0);
    glPointSize(15.0);
    glBegin(GL_POINTS);
    glVertex2d(PREV_X, WINDOW_HEIGHT - PREV_Y - 1);
    glEnd();

    int color = 0;
    for(int j=0; j<invader_rows; j++){
        if(j % 2 == 0 && j != 0) color++;
        for(int i=0; i<9; i++){
            if(game == 0) game += gameover(&inv[j * 9 + i]);
            
            if(color % 3 == 0) glColor3d(1.0, 0.0, 0.0);
            else if(color % 3 == 1) glColor3d(0.0, 1.0, 0.0);
            else glColor3d(0.0, 0.0, 1.0);

            summon_invader(4, &inv[j * 9 + i]);
            if(game == 0) move_invader(speed,&inv[j * 9 + i]);
            kill_invader(list, &inv[j * 9 + i], 4);
            explode_invader(&inv[j * 9 + i]);
        }
    }

    //ビームを動かす
    list_node *iter = list.root;
    while(iter != NULL) {
        glColor3d(1.0, 1.0, 1.0);
        glLineWidth(5.0);
        glBegin(GL_LINES);
        glVertex2d(iter->x, iter->y);
        glVertex2d(iter->x, iter->y + 20);
        glEnd();
        iter->y += 8 + beam_spead;
        iter = iter->next;
    }

    //破片を動かす
    list_node *iter_debris_pos = list_debris_pos.root;
    list_node *iter_debris_dif = list_debris_dif.root;
    while(iter_debris_pos != NULL || iter_debris_dif != NULL) {
        glColor3d(1.0, 1.0, 1.0);
        glPointSize(6.0);
        glBegin(GL_POINTS);
        double x_temp = (double)(iter_debris_pos->x) * 1e-6;
        double y_temp = (double)(iter_debris_pos->y) * 1e-6;
        glVertex2d((int)(x_temp), (int)(y_temp));
        glEnd();
        if(game == 0) move_debris(iter_debris_pos, iter_debris_dif);
        iter_debris_pos = iter_debris_pos->next;
        iter_debris_dif = iter_debris_dif->next;
    }


    //この上をいじる
    
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
    TIME += 0.010;
    TIME_INV += 1;
    // 再描画
    glutPostRedisplay();

    // 10ミリ秒後にtimer関数を引数0で自分自身を呼び出す
    glutTimerFunc(10, timer, 0);
}

void mouse(int button, int state, int x, int y) {
    // ビーム発射用
    if (y >= 450 && y <750 && x >= 0 && x < WINDOW_WIDTH && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && game == 0) {
        if(list.root == NULL) {
            list_node *node = NULL;
            node->x = PREV_X;
            node->y = WINDOW_HEIGHT - PREV_Y - 1;
            list.root = node;
        } else {
            add_node(&list, PREV_X, WINDOW_HEIGHT - PREV_Y - 1);
        }
    }
    
    //画面外のビームを消す
    if(list.root->y > 750) {
        remove_node(&list, list.root);
    } 
    //画面外の破片を消す
    list_node *iter_debris_pos = list_debris_pos.root;
    list_node *iter_debris_dif = list_debris_dif.root;
    while(iter_debris_pos != NULL || iter_debris_dif != NULL) {
        if(iter_debris_pos->y * 1e-6 < -20) {
            list_node *iter_debris_pos_temp = iter_debris_pos;
            list_node *iter_debris_dif_temp = iter_debris_dif;
            iter_debris_pos = iter_debris_pos->next;
            iter_debris_dif = iter_debris_dif->next;
            remove_node(&list_debris_pos, iter_debris_pos_temp);
            remove_node(&list_debris_dif, iter_debris_dif_temp);
        } else {
            iter_debris_pos = iter_debris_pos->next;
            iter_debris_dif = iter_debris_dif->next;
        }
    }
}

void motion(int x, int y) {
    if (y >= 450 && y <750 && x >= 0 && x < WINDOW_WIDTH && game == 0) {
        PREV_X = x;
        PREV_Y = 600;
    }
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) {
        free(inv);
        free_list(&list);
        free_list(&list_debris_pos);
        free_list(&list_debris_dif);
        exit(1);
    }
}

void keyboard_sp(int key, int x, int y) {
    if (key == GLUT_KEY_UP && beam_spead < 6) {
        beam_spead += 2;
    } else if (key == GLUT_KEY_DOWN && beam_spead > -6) {
        beam_spead -= 2;
    }
}

int main(int argc, char **argv) {
    init_list(&list);
    int n = atoi(argv[1]);
    speed = game_difficulty_speed(n);
    invader_rows = game_difficulty_invader_rows(n);
    inv = (invader*)malloc(9 * invader_rows * sizeof(invader));
    for(int j=0; j<invader_rows; j++){
        for(int i=0; i<9; i++) init_invader(i * 75 + 100, (8-speed) * 50 - 80 * j, &inv[j * 9 + i]);
    }
    // GLUTの初期化
    glutInit(&argc, argv);
    // GLUTの機能の有効化
    glutInitDisplayMode(GLUT_DOUBLE);
    // ウィンドウのサイズを設定
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    // ウィンドウの作成 (引数はウィンドウのタイトル)
    glutCreateWindow("INVADER GAME");
    // 描画に使う関数の登録
    glutDisplayFunc(display);
    // ウィンドウのサイズ変更時に呼ばれる関数の登録
    glutReshapeFunc(reshape);
    // 100ミリ秒後にtimer関数を引数0で呼び出す
    glutTimerFunc(10, timer, 0);
    // マウス関係のコールバック関数
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboard_sp);
    // 描画ループの開始
    glutMainLoop();
}