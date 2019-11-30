#ifndef MINIMOTE_SERVER_H
#define MINIMOTE_SERVER_H

#include "minimote/controller/minimote_controller.h"
#include "buffer/rolling_buffer.h"
#include "commons/globals.h"
#include "adt/hash/hash.h"

#define MINIMOTE_SERVER_TCP_BUFFER_SIZE 4096
#define MAX_HOSTNAME_LENGTH 256

typedef struct minimote_server_t {
    int tcp_socket;
    bool tcp_running;
    int tcp_port;

    int udp_socket;
    bool udp_running;
    int udp_port;

    char hostname[MAX_HOSTNAME_LENGTH];
    hash clients;

    minimote_controller_config controller_config;
} minimote_server;

void minimote_server_init(minimote_server *server,
                          int tcp_port,
                          int udp_port,
                          minimote_controller_config config);

bool minimote_server_start_tcp(minimote_server *server);
bool minimote_server_start_udp(minimote_server *server);

void minimote_server_stop_tcp(minimote_server *server);
void minimote_server_stop_udp(minimote_server *server);

#endif // MINIMOTE_SERVER_H