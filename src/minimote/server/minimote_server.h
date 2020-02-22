#ifndef MINIMOTE_SERVER_H
#define MINIMOTE_SERVER_H

#include "minimote/controller/minimote_controller.h"
#include "buffer/rolling_buffer.h"
#include "commons/globals.h"
#include "adt/hash/hash.h"

#define MAX_HOSTNAME_LENGTH 256

typedef struct minimote_server_t {
    bool running;

    char hostname[MAX_HOSTNAME_LENGTH];
    hash clients; // (struct sockaddr *) -> (minimote_client *)

    minimote_controller_config controller_config;

    int tcp_socket;
    int tcp_port;

    int udp_socket;
    int udp_port;

    fd_set read_sockets;
    fd_set write_sockets;
    fd_set error_sockets;
    int max_socket_fd;
} minimote_server;

void minimote_server_init(minimote_server *server,
                          int tcp_port,
                          int udp_port,
                          minimote_controller_config config);

bool minimote_server_start(minimote_server *server);

void minimote_server_stop(minimote_server *server);

#endif // MINIMOTE_SERVER_H