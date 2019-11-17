#ifndef HASH_NODE_H
#define HASH_NODE_H

typedef struct hash_node_t {
    void * key;
    void * value;
} hash_node;

hash_node * hash_node_new(void *key, void *data);
void hash_node_free(hash_node *node);

#endif // HASH_NODE_H
