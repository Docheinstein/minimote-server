#ifndef HASH_NODE_H
#define HASH_NODE_H

typedef struct hash_node_t {
    void * key;
    void * value;
} hash_node;

hash_node * hash_node_new(void *key, void *data);

#endif // HASH_NODE_H
