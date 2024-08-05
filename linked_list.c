#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "linked_list.h"

// リストの作成
void init_list(linked_list *list) {
    list->root = NULL;
    list->size = 0;
}

// rootを先頭要素とする連結リストの末尾に新しいノードを追加
void add_node(linked_list *list, int x, int y) {
    // 新しいノードの作成
    list_node *node = (list_node*)malloc(sizeof(list_node));
    node->x = x;
    node->y = y;
    node->next = NULL;

    // 末尾要素を探す
    if (list->root == NULL) {
        // まだ要素がなければ，先頭要素を新規ノードにする
        list->root = node;
    } else {
        // あるノードの次のノード (next) がNULLになるものを探す
        list_node *iter = list->root;
        while (iter->next != NULL) {
            iter = iter->next;
        }

        // 見つかったら末尾要素を新規ノードで置き換える
        iter->next = node;
    }

    // リストの要素数を1増やす
    list->size += 1;
}

// nodeで示されるノードをlistから削除する
void remove_node(linked_list *list, list_node *node) {
    // ノードが見つかったかどうか
    bool found = false;

    // ノードを探索する
    if (list->root == node) {
        // 先頭ノードがnodeならrootを置き換える
        list->root = list->root->next;
        found = true;
    } else {
        // 次の要素がnodeとなるノードを探す
        list_node *iter = list->root;
        while (iter != NULL) {
            if (iter->next == node) {
                // もし次のノードがnodeなら，さらにその次のノードとつなぎ直す
                iter->next = iter->next->next;
                found = true;
                break;
            }
            iter = iter->next;
        }
    }

    // もしnode見つかっていたら，メモリを解放してリストの要素数を1減らす
    if (found) {
        free(node);
        list->size -= 1;
    }
}

// リストのデータを全て削除する
void free_list(linked_list *list) {
    // 先頭から順にノードをfreeしていく
    list_node *iter = list->root;
    while (iter != NULL) {
        // 先に次のノードのポインタを別の変数にコピーしてからfreeする
        list_node *next = iter->next;
        free(iter);
        iter = next;
    }

    // 最後にリスト自体をfreeする
    free(list);
}
