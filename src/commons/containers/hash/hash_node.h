#ifndef HASH_NODE_H
#define HASH_NODE_H

#include "commons/types.h"

typedef struct hash_node {
    void * key;
    void * value;
    struct hash_node * next;
} hash_node;

hash_node * hash_node_new(void *key, void *data);
void hash_node_free(hash_node *node);

#endif // HASH_NODE_H
