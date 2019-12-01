#ifndef HASH_H
#define HASH_H

#include "adt/list/list.h"
#include "adt/hash/node/hash_node.h"
#include "commons/globals.h"

typedef struct hash_t {
    uint32 capacity;
    hash_node ** buckets;
    bool (*key_eq_func)(void *, void *);
    uint32 (*hash_func)(void *);
    void (*key_free_func)(void *);
    void (*value_free_func)(void *);
} hash;

void hash_init(
        hash *hash,
        int capacity,
        uint32 (*hash_func) (void *data),
        bool (*key_eq_func) (void *k1, void *k2)
);

void hash_init_full(
        hash *hash,
        int capacity,
        uint32 (*hash_func) (void *data),
        bool (*key_eq_func) (void *k1, void *k2),
        void (*key_free_func) (void *key),
        void (*value_free_func) (void *key)
);

bool hash_put(hash *hash, void *key, void *value);
//void hash_put_if_not_exists(hash *hash, void *key, void *value);

void * hash_get(hash *hash, void *key);

//void * hash_take(hash *hash, void *key);
bool hash_delete(hash *hash, void *key);

void hash_foreach(hash *hash, void (*fun)(hash_node *node, void *), void *arg);
//void hash_foreach1(hash *hash, void (*fun)(hash_node *node, void *argument), void *arg);

#endif // HASH_H
