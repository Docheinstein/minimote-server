#include "hash.h"

#include <stdlib.h>
#include <stdio.h>
#include <logging/logging.h>

static void hash_put_cond(hash *hash, void *key, void *value, bool replace_if_exists);
static void hash_node_visitor(void *node_data, void *node_visit_fun);
static void hash_node_visitor_arg(void *node_data, void *node_visit_fun, void *arg);
static bool hash_node_key_comparator(void *node_data, void *arg1, void *arg2);
static hash_node * hash_get_node(hash *hash, void *key);

void * hash_take_cond(hash *hash, void *key, bool delete);

void hash_init(
        hash *hash,
        int capacity,
        uint32 (*hash_func)(void *),
        bool (*key_eq_func)(void *, void *)) {
    hash->capacity = capacity;
    hash->buckets = malloc(sizeof(list *) * capacity);
    for (uint32 i = 0; i < capacity; i++) {
        hash->buckets[i] = NULL;
    }
    hash->hash_func = hash_func;
    hash->key_eq_func = key_eq_func;
    hash->key_free_func = NULL;
    hash->value_free_func = NULL;
}

void hash_init_full(
        hash *hash,
        int capacity,
        uint32 (*hash_func)(void *),
        bool (*key_eq_func)(void *, void *),
        void (*key_free_func)(void *),
        void (*value_free_func)(void *)) {
    hash_init(hash, capacity, hash_func, key_eq_func);
    hash->key_free_func = key_free_func;
    hash->value_free_func = value_free_func;
}

void hash_put(hash *hash, void *key, void *value) {
    hash_put_cond(hash, key, value, true);
}

void hash_put_if_not_exists(hash *hash, void *key, void *value) {
    hash_put_cond(hash, key, value, false);
}

void * hash_get(hash *hash, void *key) {
    hash_node * hnode = hash_get_node(hash, key);
    if (!hnode)
        return NULL;
    return hnode->value;
}

void *hash_take_cond(hash *hash, void *key, bool delete) {
    uint32 bucket_index = hash->hash_func(key) % hash->capacity;

    if (!hash->buckets[bucket_index]) {
        // Bucket corresponding to this key not even initialized
        return NULL;
    }

    list_node * node = list_until2(
            hash->buckets[bucket_index], hash_node_key_comparator, hash->key_eq_func, key);

    if (!node) {
        // The bucket exists, but there are no node in there
        return NULL;
    }

    hash_node *hnode = (hash_node *) node->data;
    void * value = hnode->value;

    // Node found
    if (delete) {
        // free the key and the value of the node

        if (hash->key_free_func) {
            t("hash->key_free_func()");
            hash->key_free_func(hnode->key);
            value = NULL;
        }

        if (hash->value_free_func) {
            t("hash->value_free_func()");
            hash->value_free_func(hnode->value);
        }
    }

    list_remove(hash->buckets[bucket_index], node);
    free(hnode);

    return value;
}

void * hash_take(hash *hash, void *key) {
    return hash_take_cond(hash, key, false);
}

void hash_delete(hash *hash, void *key) {
    hash_take_cond(hash, key, true);
}


void hash_foreach(hash *hash, void (*fun)(hash_node *)) {
    for (uint32 i = 0; i < hash->capacity; i++) {
        if (!hash->buckets[i])
            // Skip empty buckets
            continue;
        list_foreach1(hash->buckets[i], hash_node_visitor, fun);
    }
}

void hash_foreach1(hash *hash, void (*fun)(hash_node *, void *), void *arg) {
    for (uint32 i = 0; i < hash->capacity; i++) {
        if (!hash->buckets[i])
            continue;
        list_foreach2(hash->buckets[i], hash_node_visitor_arg, fun, arg);

    }
}


// ----

void hash_put_cond(hash *hash, void *key, void *value, bool replace_if_exists) {
    uint32 bucket_index = hash->hash_func(key) % hash->capacity;

    if (!hash->buckets[bucket_index]) {
        // First insertion for this bucket, alloc list
        d("Allocating bucket [%d]", bucket_index);
        hash->buckets[bucket_index] = malloc(sizeof(list));
        list_init(hash->buckets[bucket_index]);
    } else {
        d("Bucket [%d] already allocated", bucket_index);
    }

    // Check whether the key already exists
    list_node * node = list_until2(
            hash->buckets[bucket_index], hash_node_key_comparator, hash->key_eq_func, key);
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
        list_append(hash->buckets[bucket_index], new_hnode);
    }
}

void hash_node_visitor(void * node_data, void *node_visit_fun) {
    void (*hash_node_visitor_fun)(hash_node *) = node_visit_fun;
    hash_node * hnode = node_data;
    hash_node_visitor_fun(hnode);
}

void hash_node_visitor_arg(void * node_data, void *node_visit_fun, void *arg) {
    void (*hash_node_visitor_fun)(hash_node *, void *) = node_visit_fun;
    hash_node * hnode = node_data;
    hash_node_visitor_fun(hnode, arg);
}

bool hash_node_key_comparator(void *node_data, void *arg1, void *arg2) {
    bool (*key_eq_func)(void *, void *) = arg1;
    void *key = arg2;
    hash_node * hnode = node_data;
    return key_eq_func(hnode->key, key);
}

hash_node * hash_get_node(hash *hash, void *key) {
    uint32 bucket_index = hash->hash_func(key) % hash->capacity;

    if (!hash->buckets[bucket_index]) {
        // Bucket corresponding to this key not even initialized
        return NULL;
    }

    list_node * node = list_until2(
            hash->buckets[bucket_index], hash_node_key_comparator, hash->key_eq_func, key);

    if (!node) {
        // The bucket exists, but there are no node in there
        return NULL;
    }

    // Node found
    hash_node * hnode = ((hash_node *) node->data);
    return hnode;
}
