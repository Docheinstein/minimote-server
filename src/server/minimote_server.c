#include "minimote_server.h"
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "client/minimote_client.h"
#include "commons/types.h"
#include "commons/utils/byte_utils.h"
#include "commons/utils/net_utils.h"
#include "commons/utils/str_utils.h"
#include "commons/utils/time_utils.h"
#include "commons/utils/typing_utils.h"
#include "controller/minimote_controller.h"
#include "log/log.h"
#include "packet/minimote_packet.h"
#include "packet/minimote_packet_type.h"

#define MAX_CLIENTS 8
#define MAX_PACKET_LENGTH 64
#define MAX_HOTKEY_LENGTH 16
#define CLIENT_SOCKET_TIMEOUT 600

volatile sig_atomic_t minimote_server_is_running = 0;

// Hash functions

static uint32 sockaddr_in_hash_func(void *arg);
static bool sockaddr_in_eq_func(void *arg1, void *arg2);
static void sockaddr_in_free_func(void *arg);
static void minimote_client_free_func(void *arg);

static void clients_iterator_check_client_socket(void *key, void *value, void *arg);

static void clients_iterator_craft_select_fds(void *key, void *value, void *arg);

// Internal helper functions

static void minimote_server_handle_tcp_ready(minimote_server *server);
static void minimote_server_handle_udp_ready(minimote_server *server);
static bool minimote_server_handle_client_ready(minimote_server *server, minimote_client *client);

static void minimote_server_handle_packet_from_client(
        minimote_server *server,
        minimote_packet *packet,
        minimote_client *client);

static bool minimote_server_answer_ping_request(
        minimote_server *server, minimote_client *client,
        minimote_packet *packet);

static bool minimote_server_answer_discover_request(
        minimote_server *server, minimote_client *client,
        minimote_packet *packet);

static minimote_client * minimote_server_get_or_put_client(
        minimote_server *server,
        struct sockaddr_in *client_sockaddr);

static bool minimote_server_try_handle_client_buffer(
        minimote_server *server,
        minimote_client *client);

static void minimote_server_setup_select_fds(minimote_server *server);

static ssize_t minimote_sendto(minimote_packet *packet, int sockfd, struct sockaddr_in address);


void minimote_server_init(minimote_server *server, 
                          minimote_display *display,
                          minimote_controller_config controller_config,
                          minimote_server_config server_config) {
    server->display = display;
    server->controller_config = controller_config;

    server->tcp_socket = -1;
    server->tcp_port = server_config.tcp_port;

    server->udp_socket = -1;
    server->udp_port = server_config.udp_port;

    FD_ZERO(&server->read_sockets);
    FD_ZERO(&server->write_sockets);
    FD_ZERO(&server->error_sockets);
    server->max_socket_fd = -1;

    // Clients
    hash_init_full(
            &server->clients, MAX_CLIENTS,
            sockaddr_in_hash_func, sockaddr_in_eq_func,
            sockaddr_in_free_func, minimote_client_free_func);

    // Hostname
    int ok = gethostname(server->hostname, HOSTNAME_MAX_LENGTH);
    if (ok < 0) {
        w("Error resolving hostname");
    } else {
        d("Hostname: %s", server->hostname);
    }
}

bool minimote_server_start(minimote_server *server) {
    minimote_server_is_running = true;

    d("Starting server...");

    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(server->udp_port);

    // TCP

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

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    i("Server started");

    while (minimote_server_is_running) {
        d("Waiting for something...");

        int ready_fds_count;
        do {
            minimote_server_setup_select_fds(server);
            ready_fds_count = select(
                    server->max_socket_fd + 1,
                    &server->read_sockets,
                    &server->write_sockets,
                    &server->error_sockets,
                    &timeout);
        } while (minimote_server_is_running && !ready_fds_count);

        if (!minimote_server_is_running)
            break;

        d("Number of ready file descriptors = %d", ready_fds_count);

#if LOG_DEBUG
        for (int i = 0; i < server->max_socket_fd; i++) {
            if (FD_ISSET(i, &server->read_sockets))
                d("read_socket %d is ready", i);
            if (FD_ISSET(i, &server->write_sockets))
                d("write_socket %d is ready", i);
            if (FD_ISSET(i, &server->error_sockets))
                d("error_socket %d is ready", i);
        }
#endif // LOG_DEBUG

        if (ready_fds_count < 0) {
            e("select() bad return: %s", strerror(errno));
            continue;
        }

        // Check which socket is ready to read
        // Important bug fix: TCP clients must be handled before TCP master,
        // Otherwise the new socket_fd is added to the read_sockets even if it
        // is not ready, and will be confused with the really ready sockets

        // TCP clients
        hash_foreach(&server->clients, clients_iterator_check_client_socket, server);

        // TCP master: check for new connections
        if (FD_ISSET(server->tcp_socket, &server->read_sockets)) {
            minimote_server_handle_tcp_ready(server);
        }

        // UDP master: wait for new data (connectionless)
        if (FD_ISSET(server->udp_socket,  &server->read_sockets)) {
            minimote_server_handle_udp_ready(server);
        }
    }

    i("Server stopped");

    return true;
}


void minimote_server_destroy(minimote_server *server) {
    hash_destroy(&server->clients);
}


// -----

void minimote_server_handle_tcp_ready(minimote_server *server) {
    d("Handling new connection on TCP master...");

    // Handle new connection
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);

    int connectionfd = accept(
            server->tcp_socket,
            (struct sockaddr *) &client_address,
            &client_address_len);

    char client_ip[INET_ADDRSTRLEN];

    d("New TCP connection established with %s:%d",
      sockaddr_to_ipv4(client_address, client_ip), client_address.sin_port);

    d("Connection fd = %d", connectionfd);

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

    i("Client connected: %s:%d [%d]",
       sockaddr_to_ipv4(client_address, client_ip),
       client_address.sin_port,
       c->id);
}

void minimote_server_handle_udp_ready(minimote_server *server) {
    d("Handling data on UDP master...");

    byte buffer[MAX_PACKET_LENGTH];
    struct sockaddr_in client_address;
    socklen_t sendsize = sizeof(client_address);

    int received_buflen = (int) recvfrom(
            server->udp_socket, buffer, MAX_PACKET_LENGTH, 0,
            (struct sockaddr*) &client_address, &sendsize);

    if (received_buflen < 0) {
        w("Error on socket %d: %s", server->udp_socket, strerror(errno));
        return;
    }

    if (received_buflen == 0) {
        w("EOF on socket %d", server->udp_socket);
        return;
    }

    minimote_client *client = minimote_server_get_or_put_client(server, &client_address);

    d("Received UDP message (%d bytes) from client [%d]",
        received_buflen,
        client->id);

    minimote_packet packet;
    int res = minimote_packet_parse(&packet, buffer, received_buflen);

    if (res < 0) {
        w("Parsing of UDP message failed");
        return;
    }

    minimote_server_handle_packet_from_client(server, &packet, client);
}

bool minimote_server_handle_client_ready(minimote_server *server, minimote_client *client) {
    byte message_buffer[MAX_PACKET_LENGTH];

    d("Blocking on recv() on connection %d", client->tcp_socket_fd);
    int received_buflen = (int) recv(client->tcp_socket_fd, message_buffer, MAX_PACKET_LENGTH, 0);

    if (received_buflen < 0) {
        w("Error on socket %d: %s", client->tcp_socket_fd, strerror(errno));
        return false;
    }

    if (received_buflen == 0) {
        d("EOF, closing connection %d properly", client->tcp_socket_fd);
        return false;
    }

    d("Received TCP message (%d bytes) from client [%d] on connection {%d}",
      received_buflen,
      client->id,
      client->tcp_socket_fd);

    // Push data into the buffer
    ring_buffer_push(&client->tcp_buffer, message_buffer, received_buflen);

    // Handle all the available packets in the buffer
    while (minimote_server_try_handle_client_buffer(server, client));

    return true;
}

void minimote_server_handle_packet_from_client(
        minimote_server *server,
        minimote_packet *packet,
        minimote_client *client) {

    d("Handling packet %s from client [%d]",
            minimote_packet_type_to_string(packet->packet_type),
            client->id);

    minimote_packet_dump(packet);

    bool print_custom = false;

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
        case SCROLL_UP:
            minimote_controller_scroll_up(&client->controller);
            break;
        case SCROLL_DOWN:
            minimote_controller_scroll_down(&client->controller);
            break;
        case MOVE: {
            uint32 payload_value = bytes_to_uint32(packet->payload);
            uint8 mid = (payload_value >> 24) & 0xFF;
            uint16 x = (payload_value >> 12) & 0xFFF;
            uint16 y = payload_value & 0xFFF;
            minimote_controller_move(&client->controller, packet->event_time, mid, x, y);

            print_custom = true;
            v("[%d] %s (X: %u, Y: %u)", client->id, minimote_packet_type_to_string(packet->packet_type), x, y);
            break;
        }
        case WRITE: {
            uint32 unicode;
            if (minimote_packet_payload_length(packet) == 1)
                unicode = bytes_to_uint8(packet->payload);
            else if (minimote_packet_payload_length(packet) == 2)
                unicode = bytes_to_uint16(packet->payload);
            else
                unicode = bytes_to_uint32(packet->payload);

            minimote_controller_write(&client->controller, unicode);

            if (log_verbose_is_enabled()) {
                print_custom = true;
                char *unicode_str = unicode_to_string(unicode);
                v("[%d] %s: u%u '%s'", client->id, minimote_packet_type_to_string(packet->packet_type), unicode, unicode_str);
                free(unicode_str);
            }

            break;
        }
        case KEY_DOWN: {
            uint8 payload_value = bytes_to_uint8(packet->payload);
            minimote_controller_key_down(&client->controller, (minimote_key_type) payload_value);

            print_custom = true;
            v("[%d] %s: %s", client->id, minimote_packet_type_to_string(packet->packet_type),
              minimote_key_type_to_string((minimote_key_type) payload_value));
            break;
        }
        case KEY_UP: {
            uint8 payload_value = bytes_to_uint8(packet->payload);
            minimote_controller_key_up(&client->controller, (minimote_key_type) payload_value);

            print_custom = true;
            v("[%d] %s: %s", client->id, minimote_packet_type_to_string(packet->packet_type),
              minimote_key_type_to_string((minimote_key_type) payload_value));
            break;
        }
        case KEY_CLICK: {
            uint8 payload_value = bytes_to_uint8(packet->payload);
            minimote_controller_key_click(&client->controller, (minimote_key_type) payload_value);

            print_custom = true;
            v("[%d] %s: %s", client->id, minimote_packet_type_to_string(packet->packet_type),
              minimote_key_type_to_string((minimote_key_type) payload_value));
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

            if (log_verbose_is_enabled()) {
                print_custom = true;
                char hotkey_str[64];
                hotkey_str[0] = '\0';
                for (int i = 0; i < payload_length; i++) {
                    strappend(hotkey_str, 64, "%s", minimote_key_type_to_string(keys[i]));
                    if (i < payload_length - 1)
                        strappend(hotkey_str, 64, " + ");
                }
                v("[%d] %s: %s", client->id, minimote_packet_type_to_string(packet->packet_type), hotkey_str);
            }

            break;
        }
        case PING: {
            if (minimote_server_answer_ping_request(server, client, packet)) {
                d("Answered to PING");
            } else {
                w("Failed to answer to PING: %s", strerror(errno));
            }
            break;
        }
        case DISCOVER_REQUEST: {
            if (minimote_server_answer_discover_request(server, client, packet)) {
                d("Answered to DISCOVER");
            } else {
                w("Failed to answer to DISCOVER: %s", strerror(errno));
            }
            break;
        }
        default:
            w("Cannot handle event: %d", packet->packet_type);
    }

    if (!print_custom)
        v("[%d] %s", client->id, minimote_packet_type_to_string(packet->packet_type));
}

bool minimote_server_answer_discover_request(minimote_server *server, minimote_client *client,
                                             minimote_packet *packet) {
    d("Answering to discover request, with hostname = %s", server->hostname);

    const size_t PAYLOAD_SIZE = strlen(server->hostname);
    minimote_packet response;
    response.packet_type = DISCOVER_RESPONSE;
    response.event_time = ms();
    response.packet_length = MINIMOTE_PACKET_HEADER_SIZE + PAYLOAD_SIZE;
    response.payload = (byte *) server->hostname;

    return minimote_sendto(&response, server->udp_socket, client->address) > 0;
}

bool minimote_server_answer_ping_request(minimote_server *server, minimote_client *client,
                                         minimote_packet *packet) {
    int payload_length = minimote_packet_payload_length(packet);

    if (payload_length < 2) {
        w("Invalid PING packet length");
        return false;
    }

    uint16 pong_port = bytes_to_uint16(packet->payload);

    d("Answering to ping request at port %d", pong_port);

    minimote_packet response;
    minimote_packet_init(&response);

    response.packet_type = PONG;
    response.event_time = ms();
    response.packet_length = MINIMOTE_PACKET_HEADER_SIZE;


    // The response address has a different port (the one contained in the ping request)
    struct sockaddr_in pong_address;

    pong_address.sin_family = AF_INET;
    pong_address.sin_addr.s_addr = client->address.sin_addr.s_addr;
    pong_address.sin_port = htons(pong_port);

    d("Sending PONG response");
    return minimote_sendto(&response, server->udp_socket, pong_address) > 0;
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
        minimote_client_init(client, *client_sockaddr_copy, server->display, server->controller_config);

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

    uint32 current_buffer_length = ring_buffer_current_length(&client->tcp_buffer);
    if (current_buffer_length < MINIMOTE_PACKET_MINIMUM_SIZE) {
        d("Nothing to handle for client [%d]", client->id);
        return false;
    }

    d("The buffer of client [%d] contains enough bytes (%d)",
      client->id, current_buffer_length);

    minimote_packet packet;

    int surplus = minimote_packet_parse(
            &packet,
            ring_buffer_get(&client->tcp_buffer),
            (int) current_buffer_length);

    if (surplus < 0) {
        d("Not enough data for make a valid packet, waiting for new data...");
        return false;
    }

    d("Can handle packet (surplus = %d)", surplus);

    // Advance the needle by the byte actually used by the packet
    ring_buffer_advance(&client->tcp_buffer, packet.packet_length);

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


ssize_t minimote_sendto(minimote_packet *packet, int sockfd, struct sockaddr_in address) {
#if LOG_DEBUG
    char address_str[INET_ADDRSTRLEN];
    sockaddr_to_ipv4(address, address_str);
    d("Sending packet %s to address [%s:%d]",
        minimote_packet_type_to_string(packet->packet_type),
        address_str, address.sin_port);
#endif

    minimote_packet_dump(packet);

    byte *data = malloc(packet->packet_length * sizeof(byte));

    minimote_packet_data(packet, data);

    ssize_t res = sendto(
            sockfd, data, packet->packet_length, MSG_CONFIRM,
            (const struct sockaddr *) &address, sizeof(address));

    free(data);
    return res;
}

// Hash functions

uint32 sockaddr_in_hash_func(void *arg) {
    struct sockaddr_in *sockaddr = (struct sockaddr_in *) arg;
    return sockaddr->sin_addr.s_addr /* | sockaddr->sin_port | (sockaddr->sin_port << 16) */;
}

bool sockaddr_in_eq_func(void *arg1, void *arg2) {
    struct sockaddr_in *sockaddr1 = (struct sockaddr_in *) arg1;
    struct sockaddr_in *sockaddr2 = (struct sockaddr_in *) arg2;
    return
            sockaddr1->sin_addr.s_addr == sockaddr2->sin_addr.s_addr /* &&
            sockaddr1->sin_port == sockaddr2->sin_port */;
}

void sockaddr_in_free_func(void *arg /* (struct sockaddr_in *) */) {
    free(arg);
}

void minimote_client_free_func(void *arg /* minimote_client * */) {
    // Close TCP socket too
    minimote_client *client = (minimote_client *) arg;
    close(client->tcp_socket_fd);
    minimote_client_destroy(arg);
}

void clients_iterator_check_client_socket(void *key, void *value, void *arg) {
    minimote_server *server = (minimote_server *) arg;
    minimote_client *client = value;

    if (FD_ISSET(client->tcp_socket_fd, &server->read_sockets)) {
        d("Client [%d] is ready to read on connection {%d}",
          client->id, client->tcp_socket_fd);
        if (!minimote_server_handle_client_ready(server, client)) {
            // Remove the client, close the socket and release resources
            char client_ip[INET_ADDRSTRLEN];

            i("Client disconnected %s:%d [%d]",
              sockaddr_to_ipv4(client->address, client_ip),
              client->address.sin_port, client->id);

            hash_delete(&server->clients, (void *) &client->address);
        }
    }

    if (FD_ISSET(client->tcp_socket_fd, &server->error_sockets)) {
        d("Client [%d] exception on connection {%d}",
          client->id, client->tcp_socket_fd);
//        if (!minimote_server_handle_client_ready(server, client)) {
//            // Remove the client, close the socket and release resources
//            v("Closing connection %d associated with client %d", client->tcp_socket_fd, client->id);
//            hash_delete(&server->clients, (void *) &client->address);
//        }
    }
}

void clients_iterator_craft_select_fds(void *key, void *value, void *arg) {
    minimote_server *server = (minimote_server *) arg;
    minimote_client *client = (minimote_client *) value;

    FD_SET(client->tcp_socket_fd, &server->read_sockets);
    FD_SET(client->tcp_socket_fd, &server->error_sockets);

    server->max_socket_fd = MAX(server->max_socket_fd, client->tcp_socket_fd);
}