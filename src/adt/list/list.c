#include "list.h"

#include "stddef.h"
#include <stdio.h>
#include <malloc.h>
#include <logging/logging.h>

static void list_delete_cond(list *list, list_node *node, bool delete_data);

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

void list_remove(list *list, list_node *node) {
    list_delete_cond(list, node, false);
}

void list_delete(list *list, list_node *node) {
    list_delete_cond(list, node, true);
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

void list_foreach1(
        list *list,
        void (*fun)(void *, void *),
        void *arg) {
    list_node *it = list->head;
    while (it) {
        fun(it->data, arg);
        it = it->next;
    }
}

void list_foreach2(
        list *list,
        void (*fun)(void *, void *, void *),
        void *arg1, void *arg2) {
    list_node *it = list->head;
    while (it) {
        fun(it->data, arg1, arg2);
        it = it->next;
    }
}

void list_foreach_i(list *list, void (*fun)(void *, int)) {
    list_node *it = list->head;
    int i = 0;
    while (it) {
        fun(it->data, i);
        it = it->next;
        i++;
    }
}

void list_foreach_i1(list *list, void (*fun)(void *, int , void *), void *arg) {
    list_node *it = list->head;
    int i = 0;
    while (it) {
        fun(it->data, i, arg);
        it = it->next;
        i++;
    }
}

void list_foreach2_i(
        list *list, void (*fun)(void *, int, void *, void *),
        void *arg1, void *arg2) {
    list_node *it = list->head;
    int i = 0;
    while (it) {
        fun(it->data, i, arg1, arg2);
        it = it->next;
        i++;
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

list_node *list_until1(list *list, bool (*finder)(void *, void *), void *arg) {
    list_node *it = list->head;
    while (it) {
        if (finder(it->data, arg))
            return it;
        it = it->next;
    }
    return NULL;
}


list_node *list_until2(
        list *list, bool (*finder)(void *, void *, void *),
        void *arg1, void *arg2) {
    list_node *it = list->head;
    while (it) {
        if (finder(it->data, arg1, arg2))
            return it;
        it = it->next;
    }
    return NULL;
}

// --

void list_delete_cond(list *list, list_node *node, bool delete_data) {
    if (!node)
        return;

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

    if (delete_data && list->data_free_func) {
        t("list->data_free_func(node->data)");
        list->data_free_func(node->data);
    }

    free(node);
}