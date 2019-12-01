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

void list_remove(list *list, list_node *node); // Frees list_node but not the data
void list_delete(list *list, list_node *node); // Frees list_node and list_node->data

void list_foreach(
        list *list,
        void (*fun)(void *data));
void list_foreach1(
        list *list,
        void (*fun)(void *data, void *argument),
        void *arg);
void list_foreach2(
        list *list, void (*fun)(void *data,
                                void *argument1, void *argument2),
        void *arg1, void *arg2);
void list_foreach_i(
        list *list,
        void (*fun)(void *data, int index));
void list_foreach_i1(
        list *list,
        void (*fun)(void *data, int index, void *argument),
        void *arg);
void list_foreach_i2(
        list *list,
        void (*fun)(void *data, int index, void *argument, void *argument2),
        void *arg1, void *arg2);

list_node * list_find(list *list, void *data);

list_node * list_until(
        list *list, bool (*finder)(void *data));
list_node * list_until1(
        list *list, bool (*finder)(void *data, void *),
        void *arg);
list_node * list_until2(
        list *list, bool (*finder)(void *data, void *, void *),
        void *arg1, void *arg2);

#endif // LIST_H
