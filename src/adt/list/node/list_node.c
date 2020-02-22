#include "list_node.h"

#include <stdlib.h>

list_node * list_node_new(void *data) {
    list_node *node = malloc(sizeof(list_node));
    node->data = data;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

list_node *list_node_next(list_node *node) {
    if (node)
        return node->next;
    return node;
}

list_node *list_node_prev(list_node *node) {
    if (node)
        return node->prev;
    return node;
}

void list_node_forward(list_node **node) {
    if (node)
        *node = (*node)->next;
}

void list_node_backward(list_node **node) {
    if (node)
        *node = (*node)->prev;
}
