#include "minimote_client.h"

#define TCP_BUFFER_SIZE 8192

static int minimote_client_next_id = 1;

static int minimote_client_get_next_id() {
    return minimote_client_next_id++;
}

void minimote_client_init(
        minimote_client *client,
        struct sockaddr_in address,
        minimote_display *display,
        minimote_controller_config controller_config
) {
    client->id = minimote_client_get_next_id();
    client->address = address;

    // BUGFIX: tcp_socket_fd should be set to -1 and not 0, otherwise
    // it might happen that a UDP client has the connection id 0 which
    // is stdin
    client->tcp_socket_fd = -1;
    ring_buffer_init(&client->tcp_buffer, TCP_BUFFER_SIZE);

    minimote_controller_init(&client->controller, display, controller_config);
}

void minimote_client_destroy(minimote_client *client) {
    ring_buffer_destroy(&client->tcp_buffer);
    minimote_controller_destroy(&client->controller);
}
