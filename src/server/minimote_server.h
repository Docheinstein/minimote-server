#ifndef MINIMOTE_SERVER_H
#define MINIMOTE_SERVER_H

#include <bits/types/sig_atomic_t.h>
#include "commons/containers/hash/hash.h"
#include "commons/utils/net_utils.h"
#include "controller/minimote_controller.h"

// Must be global and sig_atomic_t to be accessed from the sigint handler
extern volatile sig_atomic_t minimote_server_is_running;

typedef struct minimote_server_config {
    int tcp_port;
    int udp_port;
} minimote_server_config;

typedef struct minimote_server {
    minimote_display *display;
    minimote_controller_config controller_config;

    char hostname[HOSTNAME_MAX_LENGTH];
    hash clients; // (struct sockaddr *) -> (minimote_client *)

    int tcp_socket;
    int tcp_port;

    int udp_socket;
    int udp_port;

    fd_set read_sockets;
    fd_set write_sockets;
    fd_set error_sockets;
    int max_socket_fd;
} minimote_server;


void minimote_server_init(
        minimote_server *server,
        minimote_display *display,
        minimote_controller_config controller_config,
        minimote_server_config server_config);

void minimote_server_destroy(minimote_server *server);

bool minimote_server_start(minimote_server *server);

#endif // MINIMOTE_SERVER_H