#ifndef MINIMOTE_SERVER_H
#define MINIMOTE_SERVER_H

#include "minimote/controller/minimote_controller.h"
#include "commons/globals.h"

#define MINIMOTE_SERVER_TCP_BUFFER_SIZE 128

typedef struct minimote_server_t {
    int tcp_socket;
    bool tcp_running;
    int tcp_port;
    byte tcp_buffer[MINIMOTE_SERVER_TCP_BUFFER_SIZE];
    int tcp_buffer_start;
    int tcp_buffer_end;

    int udp_socket;
    bool udp_running;
    int udp_port;

    minimote_controller controller;
} minimote_server;

void minimote_server_init(minimote_server *server,
                          int tcp_port,
                          int udp_port);

void minimote_server_start_tcp(minimote_server *server);
void minimote_server_start_udp(minimote_server *server);

void minimote_server_stop_tcp(minimote_server *server);
void minimote_server_stop_udp(minimote_server *server);




#endif // MINIMOTE_SERVER_H
