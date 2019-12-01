#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "commons/utils/net_utils.h"
#include "commons/utils/time_utils.h"
#include "logging/logging.h"
#include "minimote/client/minimote_client.h"
#include "commons/globals.h"
#include "commons/utils/byte_utils.h"
#include "minimote_server.h"
#include "minimote/packet/type/minimote_packet_type.h"
#include "minimote/packet/minimote_packet.h"

#define MAX_PACKET_LENGTH 64
#define MAX_HOTKEY_LENGTH 16
#define CLIENT_SOCKET_TIMEOUT 600

// Hash functions

static uint32 sockaddr_in_hash_func(void *arg);
static bool sockaddr_in_key_eq_func(void *arg1, void *arg2);
static void sockaddr_in_free_func(void *arg);
static void minimote_client_free_func(void *arg);

static void clients_iterator_check_client_socket(hash_node *hnode, void *arg);

static void clients_iterator_craft_select_fds(hash_node *hnode, void *arg);

// Internal helper functions

static void minimote_server_handle_tcp_ready(minimote_server *server);
static void minimote_server_handle_udp_ready(minimote_server *server);
static bool minimote_server_handle_client_ready(minimote_server *server, minimote_client *client);

static void minimote_server_handle_packet_from_client(
        minimote_server *server,
        minimote_packet *packet,
        minimote_client *client);

static bool minimote_server_answer_discover_request(
        minimote_server *server, minimote_client *client);

static minimote_client * minimote_server_get_or_put_client(
        minimote_server *server,
        struct sockaddr_in *client_sockaddr);

static bool minimote_server_try_handle_client_buffer(
        minimote_server *server,
        minimote_client *client);

static void minimote_server_setup_select_fds(minimote_server *server);


void minimote_server_init(minimote_server *server,
                          int tcp_port,
                          int udp_port,
                          minimote_controller_config config) {
    server->tcp_socket = -1;
    server->tcp_port = tcp_port;

    server->udp_socket = -1;
    server->udp_port = udp_port;

    FD_ZERO(&server->read_sockets);
    FD_ZERO(&server->write_sockets);
    FD_ZERO(&server->error_sockets);
    server->max_socket_fd = -1;

    server->running = false;
    server->controller_config = config;

    // Clients
    hash_init_full(
            &server->clients, 16,
            sockaddr_in_hash_func, sockaddr_in_key_eq_func,
            sockaddr_in_free_func, minimote_client_free_func);

    // Hostname
    int ok = gethostname(server->hostname, MAX_HOSTNAME_LENGTH);
    if (ok < 0) {
        w("Error resolving hostname");
    } else {
        d("Initialized minimote server, hostname = %s", server->hostname);
    }
}

void minimote_server_stop(minimote_server *server) {
    i("Stopping server");
    server->running = false;
}

bool minimote_server_start(minimote_server *server) {
    i("Starting server on port %d", server->tcp_port);
    server->running = true;

    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(server->udp_port);

    // TCP socket

    server->tcp_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server->tcp_socket < 0) {
        e("Socket creation failed: %s", strerror(errno));
        return false;
    }

    // -- SO_REUSEADDR, SO_REUSEPORT
    int reuse_addr_port_opt = 1;
    if (setsockopt(server->tcp_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
               &reuse_addr_port_opt, sizeof(reuse_addr_port_opt)) < 0) {
        e("setsockopt error: %s", strerror(errno));
    }

    // -- bind()
    if (bind(server->tcp_socket,
            (struct sockaddr *)& server_address,
            sizeof(server_address)) < 0) {
        e("Socket bind failed: %s", strerror(errno));
        return false;
    }

    // -- listen()
    if (listen(server->tcp_socket, 5) != 0) {
        e("Socket listening failed: %s", strerror(errno));
        return false;
    }

    // UDP

    server->udp_socket = socket(AF_INET, SOCK_DGRAM, 0);


    if (server->udp_socket < 0) {
        e("Socket creation failed: %s", strerror(errno));
        return false;
    }

    // -- SO_REUSEADDR, SO_REUSEPORT
    reuse_addr_port_opt = 1;
    setsockopt(server->udp_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
               &reuse_addr_port_opt, sizeof(reuse_addr_port_opt));

    // -- bind()
    if (bind(server->udp_socket, (struct sockaddr *)& server_address, sizeof(server_address)) < 0) {
        e("Socket bind failed: %s", strerror(errno));
        return false;
    }

    // select() setup: listen on TCP/UDP sockets

    while (server->running) {
        d("Building select() fds...");

        minimote_server_setup_select_fds(server);

        i("Waiting for something...");

        d("--> TCP listening: %d", FD_ISSET(server->tcp_socket, &server->read_sockets));
        d("--> UDP listening: %d", FD_ISSET(server->udp_socket, &server->read_sockets));
        d("--> MAX socket_fd: %d", server->max_socket_fd);
        d("--> TCP socket_fd: %d", server->tcp_socket);
        d("--> UDP socket_fd: %d", server->udp_socket);

        int ready_fds_count =
                select(server->max_socket_fd + 1,
                        &server->read_sockets,
                        &server->write_sockets,
                        &server->error_sockets,
                        NULL);

        if (ready_fds_count < 0) {
            e("select() bad return (%d): %s", ready_fds_count, strerror(errno));
            continue;
        }

        d("select() returned %d", ready_fds_count);

        // Check which socket is ready to read
        // Important bug fix: TCP clients must be handled before TCP master,
        // Otherwise the new socket_fd is added to the read_sockets even if it
        // is not ready, and will be confused with the really ready sockets

        // TCP clients
        hash_foreach(&server->clients, clients_iterator_check_client_socket, server);

        // TCP master
        if (FD_ISSET(server->tcp_socket, &server->read_sockets)) {
            minimote_server_handle_tcp_ready(server);
        }

        // UDP master
        if (FD_ISSET(server->udp_socket,  &server->read_sockets)) {
            minimote_server_handle_udp_ready(server);
        }
    }

    return true;
}

// -----

void minimote_server_handle_tcp_ready(minimote_server *server) {
    d("TCP socket is ready for read data");

    // Handle new connection
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);

    int connectionfd = accept(
            server->tcp_socket,
            (struct sockaddr *) &client_address,
            &client_address_len);
//    fnctl()

    char client_ip[INET_ADDRSTRLEN];

    i("Received TCP connection from %s:%d (socket fd = %d)",
           sockaddr_to_ipv4(client_address, client_ip),
           client_address.sin_port,
           connectionfd);

    // SO_RCVTIMEO
    struct timeval socket_timeout;
    socket_timeout.tv_sec = CLIENT_SOCKET_TIMEOUT;
    socket_timeout.tv_usec = 0;
    if (setsockopt(connectionfd, SOL_SOCKET, SO_RCVTIMEO,
                   &socket_timeout, sizeof(socket_timeout)) < 0) {
        w("setsockopt error: %s", strerror(errno));
    }

    // Add the client to the handled clients
    minimote_client *c = minimote_server_get_or_put_client(server, &client_address);
    c->tcp_socket_fd = connectionfd;
}

void minimote_server_handle_udp_ready(minimote_server *server) {
    d("UDP socket is ready for read data");

    byte buffer[MAX_PACKET_LENGTH];
    struct sockaddr_in client_address;
    socklen_t sendsize = sizeof(client_address);

    int received_bufflen = recvfrom(
            server->udp_socket, buffer, MAX_PACKET_LENGTH, 0,
            (struct sockaddr*) &client_address, &sendsize);

    if (received_bufflen < 0) {
        w("Error on socket %d: %s", server->udp_socket, strerror(errno));
        return;
    }

    if (received_bufflen == 0) {
        d("EOF on socket %d: %s", server->udp_socket, strerror(errno));
        return;
    }

    char client_ip[INET_ADDRSTRLEN];

    d("Received UDP message (%d bytes) from %s:%d",
           received_bufflen,
           sockaddr_to_ipv4(client_address, client_ip),
           client_address.sin_port);

    minimote_packet packet;
    int res = minimote_packet_parse(&packet, buffer, received_bufflen);

    if (res < 0) {
        w("Parsing of UDP message goes wrong");
        return;
    }

    minimote_client *client = minimote_server_get_or_put_client(server, &client_address);
    minimote_server_handle_packet_from_client(server, &packet, client);
}

bool minimote_server_handle_client_ready(minimote_server *server, minimote_client *client) {
    byte message_buffer[MAX_PACKET_LENGTH];

    d("Blocking on recv() on connection %d", client->tcp_socket_fd);
    int received_bufflen = recv(client->tcp_socket_fd, message_buffer, MAX_PACKET_LENGTH, 0);

    if (received_bufflen < 0) {
        w("Error on socket %d: %s", client->tcp_socket_fd, strerror(errno));
        return false;
    }

    if (received_bufflen == 0) {
        d("EOF, closing connection %d properly", client->tcp_socket_fd);
        return false;
    }

    d("Received TCP message (%d bytes) from client [%d] on connection %d\n",
      received_bufflen,
      client->id,
      client->tcp_socket_fd);

    // Push into the buffer
    rolling_buffer_push(&client->tcp_buffer, message_buffer, received_bufflen);

    // Handle all the available packets in the buffer
    while (minimote_server_try_handle_client_buffer(server, client));

    return true;
}

void minimote_server_handle_packet_from_client(
        minimote_server *server,
        minimote_packet *packet,
        minimote_client *client) {

    d("Handling packet from client [%d]", client->id);
    minimote_packet_dump(packet);

    switch (packet->packet_type) {
        case LEFT_DOWN:
            minimote_controller_left_down(&client->controller);
            break;
        case LEFT_UP:
            minimote_controller_left_up(&client->controller);
            break;
        case LEFT_CLICK:
            minimote_controller_left_click(&client->controller);
            break;
        case MIDDLE_DOWN:
            minimote_controller_middle_down(&client->controller);
            break;
        case MIDDLE_UP:
            minimote_controller_middle_up(&client->controller);
            break;
        case MIDDLE_CLICK:
            minimote_controller_middle_click(&client->controller);
            break;
        case RIGHT_DOWN:
            minimote_controller_right_down(&client->controller);
            break;
        case RIGHT_UP:
            minimote_controller_right_up(&client->controller);
            break;
        case RIGHT_CLICK:
            minimote_controller_right_click(&client->controller);
            break;
        case MOVE: {
            uint32 payload_value = bytes_to_uint32(packet->payload);
            uint8 mid = (payload_value >> 24) & 0xFF;
            uint16 x = (payload_value >> 12) & 0xFFF;
            uint16 y = payload_value & 0xFFF;
            minimote_controller_move(&client->controller, packet->event_time, mid, x, y);
            break;
        }
        case SCROLL_UP:
            minimote_controller_scroll_up(&client->controller);
            break;
        case SCROLL_DOWN:
            minimote_controller_scroll_down(&client->controller);
            break;
        case TYPE: {
            if (minimote_packet_payload_length(packet) > 1) {
                w("Type argument too complex, cannot handle it yet\n");
                break;
            }
            uint8 payload_value = bytes_to_uint8(packet->payload);
            minimote_controller_write(&client->controller, payload_value);
            break;
        }
        case KEY_DOWN: {
            uint8 payload_value = bytes_to_uint8(packet->payload);
            minimote_controller_key_down(&client->controller, (minimote_key_type) payload_value);
            break;
        }
        case KEY_UP: {
            uint8 payload_value = bytes_to_uint8(packet->payload);
            minimote_controller_key_up(&client->controller, (minimote_key_type) payload_value);
            break;
        }
        case KEY_CLICK: {
            uint8 payload_value = bytes_to_uint8(packet->payload);
            minimote_controller_key_click(&client->controller, (minimote_key_type) payload_value);
            break;
        }
        case HOTKEY: {
            int payload_length = minimote_packet_payload_length(packet);

            // Build a keys array
            minimote_key_type keys[MAX_HOTKEY_LENGTH];

            for (int i = 0; i < payload_length; i++) {
                keys[i] = bytes_to_uint8(&packet->payload[i]);
            }

            minimote_controller_hotkey(&client->controller, keys, payload_length);

            break;
        }
        case DISCOVER_REQUEST: {
            d("Received DISCOVER_REQUEST");

            if (minimote_server_answer_discover_request(server, client)) {
                d("Answered to DISCOVER");
            } else {
                w("Failed to answer to DISCOVER: %s", strerror(errno));
            }

            break;
        }
        default:
            w("Cannot handle event: %d", packet->packet_type);
    }
}


bool minimote_server_answer_discover_request(minimote_server *server, minimote_client *client) {
    d("Answering to discover request, with hostname = %s", server->hostname);

    const size_t PAYLOAD_SIZE = strlen(server->hostname);
    minimote_packet response;
    response.packet_type = DISCOVER_RESPONSE;
    response.event_time = current_ms();
    response.packet_length = MINIMOTE_PACKET_HEADER_SIZE + PAYLOAD_SIZE;
    response.payload = (byte *) server->hostname;

    byte packet_data[64];
    minimote_packet_data(&response, packet_data);

    return sendto(
            server->udp_socket, packet_data,response.packet_length,MSG_CONFIRM,
            (const struct sockaddr *) &client->address, sizeof(client->address)) > 0;
}

minimote_client *minimote_server_get_or_put_client(minimote_server *server, struct sockaddr_in *client_sockaddr) {

    // Retrieve the minimote_client associated with client of the request
    // Or create if it does not exist

    minimote_client *client = hash_get(&server->clients, client_sockaddr);

    if (!client) {
        char new_client_ip[INET_ADDRSTRLEN];

        d("First message of client with address %s:%d",
          sockaddr_to_ipv4(*client_sockaddr, new_client_ip),
          client_sockaddr->sin_port);

        // Make a copy of sockaddr_in in order to use it as key
        struct sockaddr_in *client_sockaddr_copy = malloc(sizeof(struct sockaddr_in));
        memcpy(client_sockaddr_copy, client_sockaddr, sizeof(struct sockaddr_in));

        // Init the client
        client = malloc(sizeof(minimote_client));
        minimote_client_init(client, *client_sockaddr_copy, server->controller_config);

        // Push the client
        hash_put(&server->clients, client_sockaddr_copy, client);
    }

    // At this point, client should be valid
    if (!client) {
        w("Minimote client should not be null at this point!");
    }

    return client;
}

bool minimote_server_try_handle_client_buffer(minimote_server *server, minimote_client *client) {

    uint32 current_buffer_length = rolling_buffer_current_length(&client->tcp_buffer);
    if (current_buffer_length < MINIMOTE_PACKET_MINIMUM_SIZE) {
        d("Nothing to handle for client [%d], ", client->id);
        return false;
    }

    d("The buffer of client [%d] contains enough bytes (%d)",
      client->id, current_buffer_length);

    minimote_packet packet;

    int surplus = minimote_packet_parse(
            &packet,
            rolling_buffer_get(&client->tcp_buffer),
            current_buffer_length);

    if (surplus < 0) {
        d("Not enough data for make a valid packet, waiting for new data...");
        return false;
    }

    d("Can handle packet (surplus = %d)", surplus);

    // Advance the needle by the byte actually used by the packet
    rolling_buffer_advance(&client->tcp_buffer, packet.packet_length);

    d("Current tcp_buffer_start = %d, tcp_buffer_end = %d\n",
      client->tcp_buffer.start, client->tcp_buffer.end);

    minimote_server_handle_packet_from_client(server, &packet, client);

    return true;
}

void minimote_server_setup_select_fds(minimote_server *server) {
    FD_ZERO(&server->read_sockets);
    FD_ZERO(&server->write_sockets);
    FD_ZERO(&server->error_sockets);

    FD_SET(server->tcp_socket, &server->read_sockets);
    FD_SET(server->udp_socket, &server->read_sockets);

    FD_SET(server->tcp_socket, &server->error_sockets);
    FD_SET(server->udp_socket, &server->error_sockets);

    server->max_socket_fd = MAX(server->tcp_socket, server->udp_socket);

    hash_foreach(&server->clients, clients_iterator_craft_select_fds, server);
}

// Hash functions

uint32 sockaddr_in_hash_func(void *arg) {
    struct sockaddr_in *sockaddr = (struct sockaddr_in *) arg;
    return sockaddr->sin_addr.s_addr | sockaddr->sin_port | (sockaddr->sin_port << 16);
}

bool sockaddr_in_key_eq_func(void *arg1, void *arg2) {
    struct sockaddr_in *sockaddr1 = (struct sockaddr_in *) arg1;
    struct sockaddr_in *sockaddr2 = (struct sockaddr_in *) arg2;
    return
            sockaddr1->sin_addr.s_addr == sockaddr2->sin_addr.s_addr &&
            sockaddr1->sin_port == sockaddr2->sin_port;
}

void sockaddr_in_free_func(void *arg /* (struct sockaddr_in *) */) {
    t();
    free(arg);
}

void minimote_client_free_func(void *arg /* minimote_client * */) {
    t();
    // Close TCP socket too
    minimote_client *client = (minimote_client *) arg;
    close(client->tcp_socket_fd);
    minimote_client_destroy(arg);
}

void clients_iterator_check_client_socket(hash_node *hnode, void *arg) {
    t();

    minimote_server *server = (minimote_server *) arg;
    minimote_client *client = hnode->value;

    if (FD_ISSET(client->tcp_socket_fd, &server->read_sockets)) {
        d("Client [%d] is ready to read on connection = %d",
          client->id, client->tcp_socket_fd);
        if (!minimote_server_handle_client_ready(server, client)) {
            // Remove the client, close the socket and release resources
            d("Closing connection %d associated with client %d", client->tcp_socket_fd, client->id);
            hash_delete(&server->clients, (void *) &client->address);
        }
    }

    if (FD_ISSET(client->tcp_socket_fd, &server->error_sockets)) {
        d("Client [%d] exception on connection = %d",
          client->id, client->tcp_socket_fd);
//        if (!minimote_server_handle_client_ready(server, client)) {
//            // Remove the client, close the socket and release resources
//            d("Closing connection %d associated with client %d", client->tcp_socket_fd, client->id);
//            hash_delete(&server->clients, (void *) &client->address);
//        }
    }
}

void clients_iterator_craft_select_fds(hash_node *hnode, void *arg) {
    minimote_server *server = (minimote_server *) arg;
    minimote_client *client = (minimote_client *) hnode->value;

    FD_SET(client->tcp_socket_fd, &server->read_sockets);
    FD_SET(client->tcp_socket_fd, &server->error_sockets);

    server->max_socket_fd = MAX(server->max_socket_fd, client->tcp_socket_fd);
}
