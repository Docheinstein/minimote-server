#include "test.h"
#include <malloc.h>
#include "commons/globals.h"
#include "logging/logging.h"
#include "adt/list/node/list_node.h"
#include "adt/list/list.h"
#include "adt/hash/hash.h"

uint32 hash_func(void *key) {
    int *ki = key;
    d("hashing %d", *ki);
    return *ki;
}

bool key_eq_func(void *k1, void *k2) {
    int *ki1 = k1;
    int *ki2 = k2;
    d("Comparing %d with %d", *ki1, *ki2);
    return *ki1 == *ki2;
}

void key_free_func(void *key) {
    free(key);
}

void value_free_func(void *val) {
    free(val);
}

void hash_iter(hash_node *node, void *arg) {
    int *iter_index = arg;
    int *k = node->key;
    int *v = node->value;
    d("[%d]  K: %d | V: %d", *iter_index, *k, *v);
    (*iter_index)++;
}

void hash_test() {
    const int N = 10;
    hash h;
    hash_init_full(
        &h,
        32,
        hash_func, key_eq_func, key_free_func, value_free_func
    );

    for (int i = 0; i < N; i++) {
        int *k = malloc(sizeof(int));
        int *v = malloc(sizeof(int));
//        *k = (((i << 16) * 5235343 ^ 293) | 0x25364) ;
        *k = i;
        *v = -i;

        d("hash_put for K = % d | V = %d", *k, *v);
        hash_put(&h, k, v);
    }

    for (int i = 0; i < N - 2; i++) {
        int *k = malloc(sizeof(int));
//        *k = (((i << 16) * 5235343 ^ 293) | 0x25364) ;
        *k = i;

        d("hash_take for K = %d", *k);
        int *v = (int *) hash_take(&h, k);
        if (v)
            d("--> took: %d", *v);
    }

    int hash_foreach_i = 0;
    hash_foreach(&h, hash_iter, &hash_foreach_i);
}

void list_test() {
    list l;
    list_init(&l);

    for (int i = 0; i < 10; i++) {
        int *d = malloc(sizeof(int));
        *d = i * 10;
        d("list_append %d", *d);
        list_prepend(&l, d);
    }

    list_node *n = NULL;
    while ((n = list_take(&l, l.head))) {
        d("Took: %d", *((int *) n->data));
    }
}

void tests() {
    hash_test();
    list_test();
}
