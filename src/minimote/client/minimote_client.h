#ifndef MINIMOTE_CLIENT_H
#define MINIMOTE_CLIENT_H

#include <minimote/controller/minimote_controller.h>
#include <netinet/in.h>
#include <buffer/rolling_buffer.h>

typedef struct minimote_client_t {
    int id;
    struct sockaddr_in address;
    int tcp_socket_fd;
    rolling_buffer tcp_buffer;
    minimote_controller controller;
} minimote_client;

void minimote_client_init(
        minimote_client *client,
        struct sockaddr_in client_socket,
        minimote_controller_config controller_config
);

#endif // MINIMOTE_CLIENT_H
