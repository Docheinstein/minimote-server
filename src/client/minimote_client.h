#ifndef MINIMOTE_CLIENT_H
#define MINIMOTE_CLIENT_H

#include <netinet/in.h>
#include "controller/minimote_controller.h"
#include "net/ring_buffer.h"

typedef struct minimote_client_t {
    int id;
    struct sockaddr_in address;
    int tcp_socket_fd;
    ring_buffer tcp_buffer;
    minimote_controller controller;
} minimote_client;

void minimote_client_init(
        minimote_client *client,
        struct sockaddr_in address,
        minimote_display *display,
        minimote_controller_config controller_config
);

void minimote_client_destroy(minimote_client *client);

#endif // MINIMOTE_CLIENT_H
