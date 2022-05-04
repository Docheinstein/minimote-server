#include "hash.h"

#include <stdio.h>
#include <stdlib.h>

static hash_node * hash_get_node(hash *hash, void *key);
static void * hash_take_or_delete(hash *hash, void *key, bool delete, bool *found);

void hash_init(
        hash *hash,
        int capacity,
        uint32 (*hash_func)(void *),
        bool (*key_eq_func)(void *, void *)) {
    hash->capacity = capacity;
    hash->buckets = malloc(capacity * sizeof(hash_node *));
    for (uint32 i = 0; i < capacity; i++) {
        hash->buckets[i] = NULL;
    }
    hash->size = 0;
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

void hash_destroy(hash *hash) {
    for (uint32 i = 0; i < hash->capacity; i++) {
        if (!hash->buckets[i])
            // Skip empty buckets
            continue;

        hash_node *it = hash->buckets[i];
        while (it) {
            hash_node *node = it;
            it = it->next;

            // Eventually free key and value
            if (hash->key_free_func)
                hash->key_free_func(node->key);

            if (hash->value_free_func)
                hash->value_free_func(node->value);

            // Free node
            hash_node_free(node);
        }
    }

    // Free the buckets
    free(hash->buckets);
}

bool hash_put(hash *hash, void *key, void *value) {
    uint32 hashed_key = hash->hash_func(key);
    uint32 bucket_index = hashed_key % hash->capacity;

    // Check whether the key already exists in the bucket
    hash_node *node = NULL;
    hash_node *it = hash->buckets[bucket_index];
    hash_node *prev = it;

    while (it) {
        prev = it;
        if (hash->key_eq_func(it->key, key)) {
            node = it;
            break;
        }
        it = it->next;
    }

    if (node) {
        // Key already in the bucket
        // Use the same node, just replace key and val (freeing them eventually)

        // Eventually free key and value
        if (hash->key_free_func)
            hash->key_free_func(node->key);

        if (hash->value_free_func)
            hash->value_free_func(node->value);

        // Replace key and value
        node->key = key;
        node->value = value;

        return false;
    }
    else {
        // Insert new node
        node = hash_node_new(key, value);

        if (prev) {
            // There is a predecessor, link the new node
            prev->next = node;
        }
        else {
            // First node, use the node as the head of the bucket
            hash->buckets[bucket_index] = node;
        }

        ++hash->size;

        return true;
    }
}

bool hash_contains(hash *hash, void *key) {
    return hash_get_node(hash, key);
}

void * hash_get(hash *hash, void *key) {
    hash_node * hnode = hash_get_node(hash, key);
    if (!hnode)
        return NULL;
    return hnode->value;
}


bool hash_delete(hash *hash, void *key) {
    uint32 bucket_index = hash->hash_func(key) % hash->capacity;

    hash_node *node = NULL;
    hash_node *it = hash->buckets[bucket_index];
    hash_node *prev = NULL;

    while (it) {
        if (hash->key_eq_func(it->key, key)) {
            node = it;
            break;
        }
        prev = it;
        it = it->next;
    }

    if (!node)
        // Node not found
        return false;

    // Node found

    // Eventually free key and value
    if (hash->value_free_func)
        hash->value_free_func(node->value);

    if (hash->key_free_func)
        hash->key_free_func(node->key);

    // Unlink
    if (prev) {
        // There is a predecessor
        prev->next = node->next;
    }
    else {
        // Last node of the bucket
        hash->buckets[bucket_index] = node->next;
    }

    hash_node_free(node);

    --hash->size;

    return true;
}

void hash_foreach(hash *hash, void (*fun)(void *, void *, void *), void *arg) {
    for (uint32 i = 0; i < hash->capacity; i++) {
        if (!hash->buckets[i])
            // Skip empty buckets
            continue;

        hash_node *it = hash->buckets[i];
        uint32 j = 0;
        while (it) {
            fun(it->key, it->value, arg);
            it = it->next;
            j++;
        }
    }
}

// _______________________________________________


hash_node * hash_get_node(hash *hash, void *key) {
    uint32 bucket_index = hash->hash_func(key) % hash->capacity;

    if (!hash->buckets[bucket_index])
        return NULL;

    hash_node *it = hash->buckets[bucket_index];
    while (it) {
        if (hash->key_eq_func(it->key, key))
            return it;
        it = it->next;
    }

    return NULL;
}