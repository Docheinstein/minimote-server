#ifndef LIST_H
#define LIST_H

#include "commons/types.h"
#include "list_node.h"

typedef struct list {
    list_node * head;
    list_node * tail;
    uint64 size;
    void (*data_free_func)(void *);
} list;

void list_init(list *list);
void list_init_full(list *list, void (*data_free_func)(void *));

void list_destroy(list *list);

void list_prepend(list *list, void *data);
void list_append(list *list, void *data);

void list_foreach(list *list, void (*fun)(void *data, void *arg), void *arg);

list_node * list_find(list *list, void *data);

#endif // LIST_H
