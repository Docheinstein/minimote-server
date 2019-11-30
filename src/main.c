#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "minimote/server/minimote_server.h"
#include "commons/utils/byte_utils.h"
#include "arguments/arguments.h"
#include "logging/logging.h"
#include "commons/conf/conf.h"

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
    // Parse arguments
    i("MinimoteServer version " MINIMOTE_SERVER_VERSION);

    d("Going to parse arguments");
    arguments args = arguments_parse(argc, argv);

    i("Initializing minimote server on port %d", args.port);
    minimote_server server;
    minimote_controller_config controller_config = minimote_controller_config_default();
    minimote_server_init(&server, args.port, args.port, controller_config);

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