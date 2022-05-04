#include "hash_node.h"

#include "stdlib.h"

hash_node *hash_node_new(void *key, void *value) {
    hash_node *node = malloc(sizeof(hash_node));
    node->key = key;
    node->value = value;
    node->next = NULL;
    return node;
}

void hash_node_free(hash_node *node) {
    free(node);
}
