#include "hash_node.h"

#include "stdlib.h"

hash_node *hash_node_new(void *key, void *value, uint32 hash) {
    hash_node *node = malloc(sizeof(hash_node));
    node->key = key;
    node->value = value;
    node->hash = hash;
    node->next = NULL;
    return node;
}