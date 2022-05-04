#ifndef HASH_H
#define HASH_H

#include "commons/containers/list/list_node.h"
#include "commons/types.h"
#include "hash_node.h"

typedef struct hash {
    /* Number of buckets */
    uint32 capacity;

    /* Buckets (each bucket is a linked list of hash_node) */
    hash_node ** buckets;

    /* Current number of nodes */
    uint64 size;

    /* Key equality function (check whether two keys are equals) */
    bool (*key_eq_func)(void *, void *);

    /* Hash function (compute an integer value for a given key) */
    uint32 (*hash_func)(void *);

    /* Key free function */
    void (*key_free_func)(void *);

    /* Value free function */
    void (*value_free_func)(void *);
} hash;

/* Initialize the hash.
 * key_free_func and value_free_func are set to NULL. */
void hash_init(
        hash *hash,
        int capacity,
        uint32 (*hash_func) (void *data),
        bool (*key_eq_func) (void *k1, void *k2)
);

/* Initialize the hash. */
void hash_init_full(
        hash *hash,
        int capacity,
        uint32 (*hash_func) (void *data),
        bool (*key_eq_func) (void *k1, void *k2),
        void (*key_free_func) (void *key),
        void (*value_free_func) (void *key)
);

/* Destroy the hash and its nodes.
 * The key and value of each node are eventually freed, if key_free_func
 * and value_free_func have been supplied with hash_init_full. */
void hash_destroy(hash *hash);

/* Insert a new pair of key and value in the hash.
 * If the key already exists in the hash, the old key and the value
 * are replaced with the new key and value.
 * The old key and value are eventually freed, if key_free_func and value_free_func
 * have been supplied with hash_init_full.
 * Returns true if the key did not exist yet.
 * */
bool hash_put(hash *hash, void *key, void *value);

/* Returns true if the key is present. */
bool hash_contains(hash *hash, void *key);

/* Returns the value associated with the given key
 * or NULL if such key is not present. */
void * hash_get(hash *hash, void *key);

/* Calls the given function for each node in the hash. */
void hash_foreach(hash *hash, void (*fun)(void *key, void *value, void *arg), void *arg);

/* Delete the node associated with the given key.
 * The key and value are eventually freed, if key_free_func and value_free_func
 * have been supplied with hash_init_full. */
bool hash_delete(hash *hash, void *key);


#endif // HASH_H
