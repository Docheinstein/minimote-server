#include "hash_node.h"

#include "stdlib.h"

hash_node *hash_node_new(void *key, void *value) {
    hash_node *node = malloc(sizeof(hash_node));
    node->key = key;
    node->value = value;
    return node;
}