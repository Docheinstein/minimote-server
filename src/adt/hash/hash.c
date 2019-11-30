#include "hash.h"

#include <stdlib.h>
#include <stdio.h>
#include <logging/logging.h>

static void hash_put_cond(hash *hash, void *key, void *value, bool replace_if_exists);
static void hash_node_visitor(void *node_data, void *node_visit_fun);
static void hash_node_visitor1(void *node_data, void *node_visit_fun, void *arg);

void hash_init(
        hash *hash,
        int capacity,
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

void hash_put(hash *hash, void *key, void *value) {
    hash_put_cond(hash, key, value, true);
}

void hash_put_if_not_exists(hash *hash, void *key, void *value) {
    hash_put_cond(hash, key, value, false);
}

void * hash_get(hash *hash, void *key) {
    uint32 pos = hash->hash_func(key) % hash->capacity;

    if (!hash->container[pos]) {
        // Bucket corresponding to this key not even initialized
        return NULL;
    }

    list_node * node = list_until2(
            hash->container[pos], hash_node_finder, hash->key_eq_func, key);

    if (!node) {
        // The bucket exists, but there are no node in there
        return NULL;
    }

    // Found
    hash_node * hnode = ((hash_node *) node->data);
    return hnode->value;
}

void hash_foreach(hash *hash, void (*fun)(hash_node *)) {
    for (uint32 i = 0; i < hash->capacity; i++) {
        if (!hash->container[i])
            // Skip empty buckets
            continue;
        list_foreach1(hash->container[i], hash_node_visitor, fun);
    }
}

void hash_foreach1(hash *hash, void (*fun)(hash_node *, void *), void *arg) {
    for (uint32 i = 0; i < hash->capacity; i++) {
        if (!hash->container[i])
            continue;
        list_foreach2(hash->container[i], hash_node_visitor1, fun, arg);

    }
}

// ----

void hash_put_cond(hash *hash, void *key, void *value, bool replace_if_exists) {
    uint32 pos = hash->hash_func(key) % hash->capacity;

    if (!hash->container[pos]) {
        // First insertion for this bucket, alloc list
        d("Allocating bucket [%d]", pos);
        hash->container[pos] = malloc(sizeof(list));
        list_init(hash->container[pos]);
    } else {
        d("Bucket [%d] already allocated", pos);
    }

    // Check whether the key already exists
    list_node * node = list_until2(
            hash->container[pos], hash_node_finder, hash->key_eq_func, key);
    if (node) {
        if (replace_if_exists) {
            hash_node * current_hnode = (hash_node *) node->data;
            d("Replacing value of already existing node");
            current_hnode->value = value;
        } else {
            d("Not replacing value of already existing node");
        }
    }
    else {
        hash_node * new_hnode = hash_node_new(key, value);
        d("Inserting new node with this key");
        list_append(hash->container[pos], new_hnode);
    }
}

void hash_node_visitor(void * node_data, void *node_visit_fun) {
    void (*hash_node_visitor_fun)(hash_node *) = node_visit_fun;
    hash_node * hnode = node_data;
    hash_node_visitor_fun(hnode);
}

void hash_node_visitor1(void * node_data, void *node_visit_fun, void *arg) {
    void (*hash_node_visitor_fun)(hash_node *, void *) = node_visit_fun;
    hash_node * hnode = node_data;
    hash_node_visitor_fun(hnode, arg);
}