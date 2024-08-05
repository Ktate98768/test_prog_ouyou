#include <stdio.h>
#include "linked_list.h"

int main(int argc, char **argv) {
    // リストの初期化
    linked_list list;
    init_list(&list);

    // リストに頂点を挿入
    add_node(&list, 1, 2);
    add_node(&list, 2, 3);
    add_node(&list, 3, 4);
    add_node(&list, 4, 5);

    // リストの内容を表示
    printf("Size: %d\n", list.size);
    for (list_node *it = list.root; it != NULL; it = it->next) {
        printf("%d, %d\n", it->x, it->y);
    }
    printf("\n");

    // リストから要素を削除 (3番目の要素を削除)
    list_node *remove = list.root;  // 先頭の要素
    remove = remove->next;  // 2番目の要素
    remove = remove->next;  // 3番目の要素
    remove_node(&list, remove);

    // リストの内容を表示
    printf("Size: %d\n", list.size);
    for (list_node *it = list.root; it != NULL; it = it->next) {
        printf("%d, %d\n", it->x, it->y);
    }

    // リストの破棄
    free_list(&list);
}