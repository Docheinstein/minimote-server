#include "minimote_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "minimote_utils.h"

static void * start_udp_server(void *arg) {
    minimote_server * server = (minimote_server *) arg;
    minimote_server_start_udp(server);

    return NULL;
}

static void * start_tcp_server(void *arg) {
    minimote_server * server = (minimote_server *) arg;
    minimote_server_start_tcp(server);

    return NULL;
}

int main(int argc, char *argv[]) {
    minimote_server server;
    minimote_server_init(&server, 50500, 50500);

    pthread_t thread_udp, thread_tcp;

    if (pthread_create(&thread_udp, NULL, start_udp_server, &server) != 0) {
        fprintf(stderr, "Thread creation failed\n");
        exit(-1);
    }

    if (pthread_create(&thread_tcp, NULL, start_tcp_server, &server) != 0) {
        fprintf(stderr, "Thread creation failed\n");
        exit(-1);
    }

    pthread_join(thread_udp, NULL);
    pthread_join(thread_tcp, NULL);
}