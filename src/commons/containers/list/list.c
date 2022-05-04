#include "list.h"
#include <stddef.h>

void list_init(list *list) {
    list->head = list->tail = NULL;
    list->size = 0;
    list->data_free_func = NULL;
}

void list_init_full(list *list, void (*data_free_func)(void *)) {
    list_init(list);
    list->data_free_func = data_free_func;
}

void list_destroy(list *list) {
    list_node *it = list->head;
    while (it) {
        list_node *node = it;
        it = it->next;
        if (list->data_free_func)
            list->data_free_func(node->data);
        list_node_free(node);
    }
}

void list_prepend(list *list, void *data) {
    list_node *node = list_node_new(data);
    if (list->head) {
        // Standard case
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
    } else {
        // Empty list, first element
        list->head = list->tail = node;
    }
    ++list->size;
}

void list_append(list *list, void *data) {
    list_node *node = list_node_new(data);
    if (list->tail) {
        // Standard case
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
    } else {
        // Empty list, first element
        list->head = list->tail = node;
    }
    ++list->size;
}

void list_foreach(list *list, void (*fun)(void *, void *), void *arg) {
    list_node *it = list->head;
    while (it) {
        fun(it->data, arg);
        it = it->next;
    }
}

list_node *list_find(list *list, void *data) {
    list_node *it = list->head;
    while (it) {
        if (data == it->data)
            return it;
        it = it->next;
    }

    return NULL;
}