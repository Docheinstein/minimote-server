#ifndef HASH_NODE_H
#define HASH_NODE_H

#include "commons/globals.h"

typedef struct hash_node_t {
    void * key;
    void * value;
    struct hash_node_t * next;
} hash_node;

hash_node * hash_node_new(void *key, void *data);

#endif // HASH_NODE_H
