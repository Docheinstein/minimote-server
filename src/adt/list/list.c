#include "list.h"

#include <stddef.h>
#include <malloc.h>
#include <logging/logging.h>

static bool list_take_or_delete(list *list, list_node *node, bool delete);

void list_init(list *list) {
    list->head = list->tail = NULL;
    list->size = 0;
    list->data_free_func = NULL;
}

void list_init_full(list *list, void (*data_free_func)(void *)) {
    list_init(list);
    list->data_free_func = data_free_func;
}

uint64 list_size(list *list) {
    return list->size;
}

list_node *list_head(list *list) {
    return list->head;
}

list_node *list_tail(list *list) {
    return list->tail;
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

list_node * list_take(list *list, list_node *node) {
    return list_take_or_delete(list, node, false) ? node : NULL;
}

bool list_delete(list *list, list_node *node) {
    return list_take_or_delete(list, node, true);
}

void list_foreach(
        list *list,
        void (*fun)(void *)) {
    list_node *it = list->head;
    while (it) {
        fun(it->data);
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

list_node *list_until(list *list, bool (*finder)(void *)) {
    list_node *it = list->head;
    while (it) {
        if (finder(it->data))
            return it;
        it = it->next;
    }

    return NULL;
}

// --

bool list_take_or_delete(list *list, list_node *node, bool delete) {
    if (!node)
        return false;

    if (node->prev) {
        node->prev->next = node->next;
    }
    else {
        list->head = node->next;
    }
    if (node->next) {
        node->next->prev = node->prev;
    }
    else {
        list->tail = node->prev;
    }

    node->next = node->prev = NULL;
    list->size--;

    if (delete && list->data_free_func) {
        d("list->data_free_func()");
        list->data_free_func(node->data);
    }

    free(node);
    return true;
}