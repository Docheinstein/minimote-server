#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <commons/utils/net_utils.h>
#include <commons/utils/time_utils.h>
#include <logging/logging.h>
#include <minimote/client/minimote_client.h>

#include "commons/globals.h"
#include "commons/utils/byte_utils.h"
#include "minimote_server.h"
#include "minimote/packet/type/minimote_packet_type.h"
#include "minimote/packet/minimote_packet.h"

#define MAX_PACKET_LENGTH 64
#define HOTKEY_INTER_KEYS_TIMEOUT_MS 5

// Hash functions for clients

static uint32 sockaddr_in_hash_func(void *arg);
static bool sockaddr_in_key_eq_func(void *arg1, void *arg2);

// Internal helper functions

static void minimote_server_handle_packet_from_client(
        minimote_server *server,
        minimote_packet *packet,
        minimote_client *client);

static bool minimote_server_answer_discover_request(
        minimote_server *server, minimote_client *client);

static minimote_client * minimote_server_get_client(
        minimote_server *server,
        struct sockaddr_in *client_sockaddr);

static bool minimote_server_try_handle_client_buffer(
        minimote_server *server,
        minimote_client *client);

typedef struct minimote_server_handle_tcp_client_arg_t {
    minimote_server *server;
    minimote_client *client;
} minimote_server_handle_tcp_client_arg;

static void * minimote_server_handle_tcp_client(void *arg);

void minimote_server_init(minimote_server *server,
                          int tcp_port,
                          int udp_port,
                          minimote_controller_config config) {
    server->tcp_socket = -1;
    server->tcp_running = 0;
    server->tcp_port = tcp_port;

    server->udp_socket = -1;
    server->udp_running = 0;
    server->udp_port = udp_port;

    server->controller_config = config;
    hash_init(&server->clients, 16, sockaddr_in_hash_func, sockaddr_in_key_eq_func);

    int ok = gethostname(server->hostname, MAX_HOSTNAME_LENGTH);
    if (ok < 0) {
        w("Error resolving hostname");
    } else {
        d("Initialized minimote server, hostname = %s", server->hostname);
    }
}

bool minimote_server_start_tcp(minimote_server *server) {
    i("Starting TCP server on port %d", server->tcp_port);
    server->tcp_running = true;

    struct sockaddr_in server_address;

    server->tcp_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server->tcp_socket < 0) {
        e("Socket creation failed: %s", strerror(errno));
        return false;
    }

    int opt = 1;
    setsockopt(server->tcp_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
               &opt, sizeof(opt));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(server->udp_port);

    if (bind(server->tcp_socket, (struct sockaddr *)& server_address, sizeof(server_address)) < 0) {
        e("Socket bind failed: %s", strerror(errno));
        return false;
    }

    if (listen(server->tcp_socket, 1) != 0) {
        e("Socket listening failed: %s", strerror(errno));
        return false;
    }

    while (server->tcp_running) {
        // Handle connections
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);

        i("Waiting for TCP connections...");

        int connectionfd = accept(
                server->tcp_socket,
                (struct sockaddr *) &client_address,
                        &client_address_len);

        char client_ip[INET_ADDRSTRLEN];

        i("Received TCP connection from %s:%d (socket fd = %d)",
               sockaddr_to_ipv4(client_address, client_ip),
               client_address.sin_port,
               connectionfd);

        pthread_t thread_client_tcp;

        minimote_server_handle_tcp_client_arg *arg = malloc(sizeof(minimote_server_handle_tcp_client_arg));
        arg->server = server;
        arg->client = minimote_server_get_client(server, &client_address);
        arg->client->tcp_socket_fd = connectionfd;

        if (pthread_create(&thread_client_tcp, NULL, minimote_server_handle_tcp_client, arg) != 0) {
            e("Thread creation failed, not handling connection");
            continue;
        }
    }

    return true;
}

bool minimote_server_start_udp(minimote_server *server) {
    i("Starting UDP server on port %d", server->udp_port);
    server->udp_running = 1;

    struct sockaddr_in server_address;

    server->udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if (server->udp_socket < 0) {
        e("Socket creation failed: %s", strerror(errno));
        return false;
    }

    int opt = 1;
    setsockopt(server->udp_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
               &opt, sizeof(opt));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(server->udp_port);

    if (bind(server->udp_socket, (struct sockaddr *)& server_address, sizeof(server_address)) < 0) {
        e("Socket bind failed: %s", strerror(errno));
        return false;
    }

    byte buffer[MAX_PACKET_LENGTH];

    while (server->udp_running) {
        bzero(buffer, MAX_PACKET_LENGTH);

        d("Waiting for UDP messages...");

        struct sockaddr_in client_address;
        socklen_t sendsize = sizeof(client_address);

        int received_bufflen = recvfrom(
                server->udp_socket, buffer, MAX_PACKET_LENGTH, 0,
                (struct sockaddr*) &client_address, &sendsize);

        if (received_bufflen <= 0) {
            w("Error in recvfrom\n");
            continue;
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
            continue;
        }

        minimote_client *client = minimote_server_get_client(server, &client_address);
        minimote_server_handle_packet_from_client(server, &packet, client);
    }

    return true;
}

void minimote_server_stop_tcp(minimote_server *server) {
    i("Stopping TCP server");
    server->tcp_running = false;
}

void minimote_server_stop_udp(minimote_server *server) {
    i("Stopping UDP server");
    server->udp_running = false;
}

// -----

bool sockaddr_in_key_eq_func(void *arg1, void *arg2) {
    struct sockaddr_in *sockaddr1 = (struct sockaddr_in *) arg1;
    struct sockaddr_in *sockaddr2 = (struct sockaddr_in *) arg2;
    return
            sockaddr1->sin_addr.s_addr == sockaddr2->sin_addr.s_addr &&
            sockaddr1->sin_port == sockaddr2->sin_port;
}

uint32 sockaddr_in_hash_func(void *arg) {
    struct sockaddr_in *sockaddr = (struct sockaddr_in *) arg;
    return sockaddr->sin_addr.s_addr | sockaddr->sin_port | (sockaddr->sin_port << 16);
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

            // Iterate forward and for each byte press the corresponding key down
            for (int i = 0; i < payload_length; i++) {
                uint8 key = bytes_to_uint8(&packet->payload[i]);
                minimote_controller_key_down(&client->controller, (minimote_key_type) key);
                msleep(HOTKEY_INTER_KEYS_TIMEOUT_MS);
            }

            // Iterate backward and for each byte press the corresponding key up
            for (int i = payload_length - 1; i >= 0; i--) {
                uint8 key = bytes_to_uint8(&packet->payload[i]);
                minimote_controller_key_up(&client->controller, (minimote_key_type) key);
                if (i > 0)
                    msleep(HOTKEY_INTER_KEYS_TIMEOUT_MS);
            }

            break;
        }
        case DISCOVER_REQUEST: {
            d("Received DISCOVER_REQUEST");

            if (minimote_server_answer_discover_request(server, client)) {
                d("Answered to DISCOVER");
            } else {
                w("Failed to answer to DISCOVER\n");
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
            server->udp_socket,packet_data,response.packet_length,MSG_CONFIRM,
            (const struct sockaddr *) &client->address,sizeof(client->address)) > 0;
}

void *minimote_server_handle_tcp_client(void *arg) {
    minimote_server_handle_tcp_client_arg * handle_arg = (minimote_server_handle_tcp_client_arg *) arg;

    d("Handling TCP client [%d] on connection fd = %d",
            handle_arg->client->id, handle_arg->client->tcp_socket_fd);

    byte message_buffer[MAX_PACKET_LENGTH];

    while (handle_arg->server->tcp_running) {
        bzero(message_buffer, MAX_PACKET_LENGTH);

        d("Waiting for TCP messages for client [%d] on connection %d...\n",
                handle_arg->client->id, handle_arg->client->tcp_socket_fd);

        int received_bufflen = recv(handle_arg->client->tcp_socket_fd, message_buffer, MAX_PACKET_LENGTH, 0);

        if (received_bufflen <= 0) {
            w("Error in recv(), closing connection %d\n", handle_arg->client->tcp_socket_fd);
            break;
        }

        d("Received TCP message (%d bytes) from client [%d] on connection %d\n",
                received_bufflen,
                handle_arg->client->id,
                handle_arg->client->tcp_socket_fd);

        // Push into the buffer
        rolling_buffer_push(&handle_arg->client->tcp_buffer, message_buffer, received_bufflen);

        // Handle all the available packets in the buffer
        while (minimote_server_try_handle_client_buffer(handle_arg->server, handle_arg->client));
    }

    close(handle_arg->client->tcp_socket_fd);
    free(arg);

    return NULL;
}

minimote_client *minimote_server_get_client(minimote_server *server, struct sockaddr_in *client_sockaddr) {

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
        client_sockaddr_copy->sin_addr = client_sockaddr->sin_addr;
        client_sockaddr_copy->sin_port = client_sockaddr->sin_port;

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