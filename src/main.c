#include "minimote/server/minimote_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "commons/utils/byte_utils.h"
#include <string.h>

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

void msleep(long msec) {
    struct timespec ts;

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    nanosleep(&ts, &ts);
}

int main(int argc, char *argv[]) {
    minimote_x11 x11;
    minimote_x11_init(&x11);
//    minimote_x11_key_click(&x11, 0x00e8); // egrave
//    minimote_x11_key_click(&x11, 0x0061); // a

    const char * str = "This is [ a function ], followed by { another function }";
    msleep(3000);

    for (int i = 0; i < strlen(str); i++) {
        char c = str[i];
        minimote_x11_key_click(&x11, c);
        msleep(50);
    }
    return -1;
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