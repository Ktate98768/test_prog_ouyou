#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif

typedef struct {
    int user;
    omp_lock_t lock;
} fork;

void init_fork(fork *f) {
    f->user = -1;
    omp_init_lock(&f->lock);
}

void pick_fork(fork *f, int who) {
    omp_set_lock(&f->lock);
    f->user = who;
    omp_unset_lock(&f->lock);
}

void put_fork(fork *f) {
    omp_set_lock(&f->lock);
    f->user = -1;
    omp_unset_lock(&f->lock);
}

#define N 5
#define FULL 10
int count[N];
fork forks[N];

int main() {
    for (int i = 0; i < N; i++) {
        count[i] = 0;
        init_fork(&forks[i]);
    }

    for(;;) { 
        #pragma omp parallel for
        for (int i = 0; i < N; i++) {
            // i: 1本目のフォークの番号 (=哲学者の番号)
            // j: 2本目のフォークの番号
            int j = (i + 1) % N;
            if (count[i] != FULL) {
                // 1本目のフォークを誰かが使っているか？
                if (forks[i].user == -1) {
                    // 使っていなかったらフォークを取る
                    pick_fork(&forks[i], i);
                }

                // 2本目のフォークを誰かが使っているか？
                if (forks[i].user == i && forks[j].user == -1) {
                    // 使っていなかったらフォークを取る
                    pick_fork(&forks[j], i);
                } else {
                    put_fork(&forks[i]);
                }

                // 2本のフォークを持っているか？
                if (forks[i].user == i && forks[j].user == i) {
                    // 持っていたら食事をする
                    printf("Eating: %d\n", i);
                    count[i] += 1;

                    // 一旦持っているフォークを離す
                    put_fork(&forks[i]);
                    put_fork(&forks[j]);
                }
            }
        }

        // 全員のお腹がいっぱいになっているか？
        int success = 1;
        for (int i = 0; i < N; i++) {
            if (count[i] != FULL) {
                success = 0;
            }
        }

        // 全員お腹がいっぱいなら終了
        if (success == 1) {
            break;
        }
    }
    printf("Finish!\n");
}
