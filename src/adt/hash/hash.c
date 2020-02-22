#include "hash.h"

#include <stdlib.h>
#include <stdio.h>
#include <logging/logging.h>


static hash_node * hash_get_node(hash *hash, void *key);
static void * hash_take_or_delete(hash *hash, void *key, bool delete, bool *found);

void hash_init(
        hash *hash,
        int capacity,
        uint32 (*hash_func)(void *),
        bool (*key_eq_func)(void *, void *)) {
    hash->capacity = capacity;
    hash->buckets = malloc(sizeof(hash_node *) * capacity);
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
        d("hash_put: key already in the bucket, replacing node key/val");

        if (hash->key_free_func) {
            d("hash_put: freeing key");
            hash->key_free_func(node->key);
        }

        if (hash->value_free_func) {
            d("hash_put: freeing value");
            hash->value_free_func(node->value);
        }

        d("hash_put: setting key/val");
        node->key = key;
        node->value = value;

        return false;
    }
    else {
        d("hash_put: inserting new node with this key");
        node = hash_node_new(key, value);

        if (prev) {
            // There is a predecessor, link the new node
            d("hash_put: linking prev with this node");
            prev->next = node;
        }
        else {
            // First node, use the node as the head of the bucket
            d("hash_put: inserting first node of the bucket [%d]", bucket_index);
            hash->buckets[bucket_index] = node;
        }

        return true;
    }
}

void * hash_get(hash *hash, void *key) {
    hash_node * hnode = hash_get_node(hash, key);
    if (!hnode)
        return NULL;
    return hnode->value;
}

void *hash_take(hash *hash, void *key) {
    return hash_take_or_delete(hash, key, false, NULL);
}

bool hash_delete(hash *hash, void *key) {
    bool found;
    hash_take_or_delete(hash, key, true, &found);
    return found;
}

void hash_foreach(hash *hash, void (*fun)(hash_node *, void *), void *arg) {
    for (uint32 i = 0; i < hash->capacity; i++) {
        d("Inspecting bucket %d", i);
        if (!hash->buckets[i])
            // Skip empty buckets
            continue;

        hash_node *it = hash->buckets[i];
        while (it) {
            fun(it, arg);
            it = it->next;
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


void * hash_take_or_delete(hash *hash, void *key, bool delete, bool *found) {
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

    if (!node) {
        d("hash_take_or_delete: node not found");
        if (found)
            *found = false;
        return NULL;
    }

    d("hash_take_or_delete: node found");
    if (found)
        *found = true;

    // Node found
    void *node_value = node->value;

    // Free the key anyhow
    if (hash->key_free_func) {
        d("hash->key_free_func()");
        hash->key_free_func(node->key);
    }

    // Free the value only if delete is required, otherwise it shall be
    // returned and thus not freed
    if (delete) {
        if (hash->value_free_func) {
            d("hash->value_free_func()");
            hash->value_free_func(node->value);
        }
    }

    if (prev) {
        d("hash_take_or_delete: unlinking, not last in bucket");
        prev->next = NULL;
    }
    else {
        d("hash_take_or_delete: unlinking, last in bucket");
        hash->buckets[bucket_index] = NULL;
    }

    free(node);

    return node_value;
}