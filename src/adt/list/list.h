#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include "commons/globals.h"
#include "adt/list/node/list_node.h"

typedef struct list_t {
    list_node * head;
    list_node * tail;
    uint64 size;
    void (*data_free_func)(void *);
} list;

void list_init(list *list);
void list_init_full(list *list, void (*data_free_func)(void *));

uint64 list_size(list *list);
list_node * list_head(list *list);
list_node * list_tail(list *list);

void list_prepend(list *list, void *data);
void list_append(list *list, void *data);

list_node * list_take(list *list, list_node *node);
bool list_delete(list *list, list_node *node);

void list_foreach(
        list *list,
        void (*fun)(void *data));

list_node * list_find(list *list, void *data);

list_node * list_until(
        list *list, bool (*finder)(void *data));

#endif // LIST_H
