#ifndef MINIMOTE_SERVER_H
#define MINIMOTE_SERVER_H

#include "minimote/controller/minimote_controller.h"
#include "commons/globals.h"

typedef struct minimote_server_t {
    bool tcp_running;
    bool udp_running;
    int tcp_port;
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
