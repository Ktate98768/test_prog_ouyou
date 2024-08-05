#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

typedef struct list_node_ {
    int x, y;
    struct list_node_ *next;
} list_node;

typedef struct {
    list_node *root;
    int size;
} linked_list;

void init_list(linked_list *list);
void add_node(linked_list *list, int x, int y);
void remove_node(linked_list *list, list_node *node);
void free_list(linked_list *list);

#endif