#include "hash.h"

#include <stdlib.h>
#include <stdio.h>
#include <logging/logging.h>


static void hash_put_or_replace(hash *hash, void *key, void *value, bool replace);
//static void hash_node_visitor(void *node_data, void *node_visit_fun);
//static void hash_node_visitor_arg(void *node_data, void *node_visit_fun, void *arg);
static bool hash_node_key_comparator(void *node_data, void *arg1, void *arg2);
static hash_node * hash_get_node(hash *hash, void *key);

static void * hash_take_cond(hash *hash, void *key, bool delete);



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
        d("Node already in the bucket");

        if (hash->key_free_func) {
            d("Freeing key");
            hash->key_free_func(node->key);
        }

        if (hash->value_free_func) {
            d("Freeing value");
            hash->value_free_func(node->value);
        }

        d("Setting new key and value for existing node");
        node->key = key;
        node->value = value;

        return false;
    }
    else {
        d("Node not in the bucket, inserting new node with this key");
        node = hash_node_new(key, value, hashed_key);

        if (prev)
            // There is a predecessor, link the new node
            prev->next = node;
        else
            // First node, use the node as the head of the bucket
            hash->buckets[bucket_index] = node;

        return true;
    }
}

void * hash_get(hash *hash, void *key) {
    hash_node * hnode = hash_get_node(hash, key);
    if (!hnode)
        return NULL;
    return hnode->value;
}

void *hash_take_cond(hash *hash, void *key, bool delete) {
//    uint32 bucket_index = hash->hash_func(key) % hash->capacity;
//
//    if (!hash->buckets[bucket_index])
//        return NULL;
//
//    hash_node *node = NULL;
//    hash_node *it = hash->buckets[bucket_index];
//    hash_node *prev = NULL;
//    while (it) {
//        if (hash->key_eq_func(it->key, key)) {
//            node = it;
//            break;
//        }
//        prev = it;
//        it = it->next;
//    }
//
//    if (!node)
//        return NULL;
//
//    // Node found
//    if (delete) {
//        // free the key and the value of the node
//
//        if (hash->key_free_func) {
//            t("hash->key_free_func()");
//            hash->key_free_func(node->key);
//        }
//
//        if (hash->value_free_func) {
//            t("hash->value_free_func()");
//            hash->value_free_func(node->value);
//        }
//    }
//
//    prev->next = NULL;
//    free(node);
//
//    return value;
}
//
//void * hash_take(hash *hash, void *key) {
//    return hash_take_cond(hash, key, false);
//}

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
        return false;

    // Node found
    // free the key and the value of the node

    if (hash->key_free_func) {
        d("hash->key_free_func()");
        hash->key_free_func(node->key);
    }

    if (hash->value_free_func) {
        d("hash->value_free_func()");
        hash->value_free_func(node->value);
    }

    if (prev)
        prev->next = NULL;
    else
        hash->buckets[bucket_index] = NULL;

    free(node);

    return true;
}


void hash_foreach(hash *hash, void (*fun)(hash_node *, void *), void *arg) {
    for (uint32 i = 0; i < hash->capacity; i++) {
        t("Inspecting bucket %d", i);
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

//void hash_foreach1(hash *hash, void (*fun)(hash_node *, void *), void *arg) {
//    for (uint32 i = 0; i < hash->capacity; i++) {
//        if (!hash->buckets[i])
//            continue;
//        list_foreach2(hash->buckets[i], hash_node_visitor_arg, fun, arg);
//    }
//}


// ----

//void hash_node_visitor(void * node_data, void *node_visit_fun) {
//    void (*hash_node_visitor_fun)(hash_node *) = node_visit_fun;
//    hash_node * hnode = node_data;
//    hash_node_visitor_fun(hnode);
//}
//
//void hash_node_visitor_arg(void * node_data, void *node_visit_fun, void *arg) {
//    void (*hash_node_visitor_fun)(hash_node *, void *) = node_visit_fun;
//    hash_node * hnode = node_data;
//    hash_node_visitor_fun(hnode, arg);
//}

//bool hash_node_key_comparator(void *node_data, void *arg1, void *arg2) {
//    bool (*key_eq_func)(void *, void *) = arg1;
//    void *key = arg2;
//    hash_node * hnode = node_data;
//    return key_eq_func(hnode->key, key);
//}

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
