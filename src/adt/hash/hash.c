#include "hash.h"

#include <stdlib.h>
#include <stdio.h>

void hash_init(hash *hash, int capacity,
        uint32 (*hash_func)(void *),
        bool (*key_eq_func)(void *, void *)) {
    hash->capacity = capacity;
    hash->container = malloc(sizeof(list *) * capacity);
    for (uint32 i = 0; i < capacity; i++) {
        hash->container[i] = NULL;
    }
    hash->hash_func = hash_func;
    hash->key_eq_func = key_eq_func;
}

static bool hash_node_finder(void *node_data, void *arg1, void *arg2) {
    bool (*key_eq_func)(void *, void *) = arg1;
    void *key = arg2;
    hash_node * hnode = node_data;

    return key_eq_func(hnode->key, key);
}

static void _hash_put(hash *hash, void *key, void *value, bool replace_if_exists) {
    uint32 pos = hash->hash_func(key) % hash->capacity;

    if (!hash->container[pos]) {
        // First insertion for this bucket, alloc list
        printf("Allocating bucket [%d]\n", pos);
        hash->container[pos] = malloc(sizeof(list));
        list_init(hash->container[pos]);
    } else {
        printf("Bucket [%d] already allocated\n", pos);
    }

    // Check whether the key already exists
    list_node * node = list_until2(
            hash->container[pos], hash_node_finder, hash->key_eq_func, key);
    if (node) {
        if (replace_if_exists) {
            hash_node * current_hnode = (hash_node *) node->data;
             printf("Replacing value of already existing node\n");
            current_hnode->value = value;
        } else {
            printf("Not replacing value of already existing node\n");
        }
    }
    else {
        hash_node * new_hnode = hash_node_new(key, value);
         printf("Inserting new node with this key\n");
        list_append(hash->container[pos], new_hnode);
    }
}

void hash_put(hash *hash, void *key, void *value) {
    _hash_put(hash, key, value, true);
}

void hash_put_if_not_exists(hash *hash, void *key, void *value) {
    _hash_put(hash, key, value, false);
}

static void hash_node_visitor(void * node_data, void *arg1) {
    void (*hash_foreach_fun)(hash_node *) = arg1;
    hash_node * hnode = node_data;
    hash_foreach_fun(hnode);
}

static void hash_node_visitor1(void * node_data, void *arg1, void *arg2) {
    void (*hash_foreach_fun)(hash_node *, void *) = arg1;
    hash_node * hnode = node_data;
    hash_foreach_fun(hnode, arg2);
}

void hash_foreach(hash *hash, void (*fun)(hash_node *)) {
    for (uint32 i = 0; i < hash->capacity; i++) {
        if (!hash->container[i])
            continue;
        list_foreach1(hash->container[i], hash_node_visitor, fun);
        printf("---\n");
    }
}

void hash_foreach1(hash *hash, void (*fun)(hash_node *, void *), void *arg) {
    for (uint32 i = 0; i < hash->capacity; i++) {
        if (!hash->container[i])
            continue;
        list_foreach2(hash->container[i], hash_node_visitor1, fun, arg);

    }
}

void * hash_get(hash *hash, void *key) {
    uint32 pos = hash->hash_func(key) % hash->capacity;

    if (!hash->container[pos]) {
        // printf("Bucket's list not even initialized");
        return NULL;
    }

    list_node * node = list_until2(
            hash->container[pos], hash_node_finder, hash->key_eq_func, key);

    if (!node) {
        // printf("No node in bucket's list");
        // Nothing found
        return NULL;
    }

    // Found
    hash_node * hnode = ((hash_node *) node->data);
    return hnode->value;
}