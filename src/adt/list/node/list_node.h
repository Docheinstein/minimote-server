#ifndef LIST_NODE_H
#define LIST_NODE_H

typedef struct list_node_t {
    struct list_node_t *next;
    struct list_node_t *prev;
    void * data;
} list_node;

list_node * list_node_new(void *data);
void list_node_free(list_node *node);

void list_node_forward(list_node **node);
void list_node_backward(list_node **node);

list_node * list_node_next(list_node *node);
list_node * list_node_prev(list_node *node);

#endif // LIST_NODE_H
