#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <commons/utils/net_utils.h>
#include <commons/utils/time_utils.h>

#include "commons/globals.h"
#include "commons/utils/byte_utils.h"
#include "minimote_server.h"
#include "minimote/packet/type/minimote_packet_type.h"
#include "minimote/packet/minimote_packet.h"

#define MAX_HOSTNAME_LENGTH 1024

static void minimote_server_handle_packet(  minimote_server *server,
                                            minimote_packet *packet,
                                            struct sockaddr_in *client);

static bool answer_discover(minimote_server * server, struct sockaddr_in *client);

static void tcp_buffer_major_swap(minimote_server *server) {
    // The current situation would be something like this
    // | -- garbabe ------------------------ (S) ---- (E) -- |
    // And will become
    // (S) ---- (E) -----------------------------------------|

    printf("Performing a buffer major swap | tcp_buffer_start = %d, tcp_buffer_end = %d\n",
           server->tcp_buffer_start, server->tcp_buffer_end);
    int current_buffer_size = server->tcp_buffer_end - server->tcp_buffer_start;

    byte tmp[MINIMOTE_SERVER_TCP_BUFFER_SIZE];
    memcpy(tmp, &server->tcp_buffer[server->tcp_buffer_start], current_buffer_size);
    // No need to memset to 0, actually
//    bzero(server->tcp_buffer, 0, MINIMOTE_SERVER_TCP_BUFFER_SIZE);
    memcpy(server->tcp_buffer, tmp, MINIMOTE_SERVER_TCP_BUFFER_SIZE);
    server->tcp_buffer_start = 0;
    server->tcp_buffer_end = current_buffer_size;

    printf("Post swap tcp_buffer_start = %d, tcp_buffer_end = %d\n",
           server->tcp_buffer_start, server->tcp_buffer_end);
}

static void push_tcp_stream_in_buffer(minimote_server *server, byte *stream, int stream_length) {
    printf("Pushing into TCP buffer %d bytes at pos %d\n", stream_length, server->tcp_buffer_end);

    if (server->tcp_buffer_end + stream_length >= MINIMOTE_SERVER_TCP_BUFFER_SIZE) {
        // The ideal would be to implemented a circular buffer, for now
        // we do a major buffer swap when the buffer reaches the end by
        // copying the current content the the beginning of the buffer
        tcp_buffer_major_swap(server);

        if (server->tcp_buffer_end + stream_length >= MINIMOTE_SERVER_TCP_BUFFER_SIZE) {
            fprintf(stderr, "TCP Buffer full anyway!!\n");
            return;
        }
    }
    // Push at the end and advance the needle
    memcpy(&server->tcp_buffer[server->tcp_buffer_end], stream, stream_length);
    server->tcp_buffer_end += stream_length;

    printf("Current tcp_buffer_start = %d, tcp_buffer_end = %d\n",
            server->tcp_buffer_start, server->tcp_buffer_end);
}

static bool try_handle_tcp_buffer(minimote_server *server) {
    if (server->tcp_buffer_end - server->tcp_buffer_start < MINIMOTE_PACKET_MINIMUM_SIZE) {
        printf("Nothing to handle yet\n");
        return false;
    }

    printf("The buffer contains enough bytes (%d), try handling from %d\n",
            server->tcp_buffer_end - server->tcp_buffer_start, server->tcp_buffer_start);

    minimote_packet packet;

    int surplus = minimote_packet_parse(
            &packet,
            &server->tcp_buffer[server->tcp_buffer_start],
            server->tcp_buffer_end - server->tcp_buffer_start);

    if (surplus < 0) {
        printf("Not enough data for make a valid packet, waiting for new data...\n");
        return false;
    }

    printf("Can handle packet (surplus = %d)\n", surplus);

    // Advance the needle by the byte actually used by the packet
    server->tcp_buffer_start += packet.expected_packet_length;

    printf("Current tcp_buffer_start = %d, tcp_buffer_end = %d\n",
           server->tcp_buffer_start, server->tcp_buffer_end);

    minimote_packet_dump(&packet);
    minimote_server_handle_packet(server, &packet, NULL);

    return true;
}

typedef struct minimote_server_handle_tcp_client_arg_t {
    minimote_server *server;
    int connectionfd;
} minimote_server_handle_tcp_client_arg;

static void * minimote_server_handle_tcp_client(void *voidarg) {
    minimote_server_handle_tcp_client_arg * arg = (minimote_server_handle_tcp_client_arg *) voidarg;

    printf("Handling TCP client connection (fd = %d)\n", arg->connectionfd);

    const int BUFFER_SIZE = 20;

    byte buffer[BUFFER_SIZE];

    while (arg->server->tcp_running) {
        memset(buffer, 0, BUFFER_SIZE);

        printf("Waiting for TCP messages on connection %d...\n", arg->connectionfd);
        int received_bufflen = recv(arg->connectionfd, buffer, BUFFER_SIZE, 0);

        if (received_bufflen <= 0) {
            fprintf(stderr, "Error in recv(), closing connection %d\n", arg->connectionfd);
            break;
        }

        printf("Received TCP message (%d bytes) from connection %d\n",
                received_bufflen,
               arg->connectionfd);

        // Push into the buffer
        push_tcp_stream_in_buffer(arg->server, buffer, received_bufflen);

        while (try_handle_tcp_buffer(arg->server));
    }

    close(arg->connectionfd);
    free(arg);

    return NULL;
}

void minimote_server_init(minimote_server *server,
                          int tcp_port,
                          int udp_port) {
    server->tcp_socket = -1;
    server->tcp_running = 0;
    server->tcp_port = tcp_port;
    memset(server->tcp_buffer, 0, MINIMOTE_SERVER_TCP_BUFFER_SIZE);
    server->tcp_buffer_start = 0;
    server->tcp_buffer_end = 0;

    server->udp_socket = -1;
    server->udp_running = 0;
    server->udp_port = udp_port;

    minimote_controller_init(&server->controller);

}

void minimote_server_start_tcp(minimote_server *server) {
    printf("Starting TCP server on port %d\n", server->tcp_port);
    server->tcp_running = true;

    struct sockaddr_in server_address;

    server->tcp_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server->tcp_socket < 0) {
        fprintf(stderr, "Socket creation failed: %s\n", strerror(errno));
        exit(-1);
    }
    int opt = 1;
    setsockopt(server->tcp_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
               &opt, sizeof(opt));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(server->udp_port);

    if (bind(server->tcp_socket, (struct sockaddr *)& server_address, sizeof(server_address)) < 0) {
        fprintf(stderr, "Socket bind failed: %s\n", strerror(errno));
        exit(-1);
    }

    if (listen(server->tcp_socket, 1) != 0) {
        fprintf(stderr, "Socket listening failed: %s\n", strerror(errno));
        exit(-1);
    }

    while (server->tcp_running) {
        // Handle clients
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);

        printf("Waiting for TCP connections...\n");

        int connectionfd = accept(server->tcp_socket, (struct sockaddr *) &client_address, &client_address_len);
        char client_ip[INET_ADDRSTRLEN];

        printf("Received TCP connection from %s:%d (fd = %d)\n",
               sockaddr_to_ipv4(client_address, client_ip),
               client_address.sin_port,
               connectionfd);

        pthread_t thread_client_tcp;

        minimote_server_handle_tcp_client_arg *arg = malloc(sizeof(minimote_server_handle_tcp_client_arg));
        arg->server = server;
        arg->connectionfd = connectionfd;

        if (pthread_create(&thread_client_tcp, NULL, minimote_server_handle_tcp_client, arg) != 0) {
            fprintf(stderr, "Thread creation failed\n");
            exit(-1);
        }
    }
}

void minimote_server_start_udp(minimote_server *server) {
    printf("Starting UDP server on port %d\n", server->udp_port);
    server->udp_running = 1;

    struct sockaddr_in server_address;

    server->udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if (server->udp_socket < 0) {
        fprintf(stderr, "Socket creation failed: %s\n", strerror(errno));
        exit(-1);
    }
    int opt = 1;
    setsockopt(server->udp_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
               &opt, sizeof(opt));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(server->udp_port);

    if (bind(server->udp_socket, (struct sockaddr *)& server_address, sizeof(server_address)) < 0) {
        fprintf(stderr, "Socket bind failed: %s\n", strerror(errno));
        exit(-1);
    }

    const int BUFFER_SIZE = 20;

    byte buffer[BUFFER_SIZE];

    while (server->udp_running) {
        memset(buffer, 0, BUFFER_SIZE);

        printf("Waiting for UDP messages...\n");

        struct sockaddr_in client_address;
        socklen_t sendsize = sizeof(client_address);

        int received_bufflen = recvfrom(
                server->udp_socket, buffer, BUFFER_SIZE, 0,
                (struct sockaddr*) &client_address, &sendsize);

        if (received_bufflen <= 0) {
            fprintf(stderr, "Error in recvfrom\n");
            exit(-1);
        }

        char client_ip[INET_ADDRSTRLEN];

        printf("Received UDP message (%d bytes) from %s:%d\n",
               received_bufflen,
               sockaddr_to_ipv4(client_address, client_ip),
               client_address.sin_port);

        minimote_packet packet;
        int res = minimote_packet_parse(&packet, buffer, received_bufflen);
        minimote_packet_dump(&packet);

        if (res < 0) {
            fprintf(stderr, "Parsing of UDP message goes wrong.\n");
            continue;
        }

        minimote_server_handle_packet(server, &packet, &client_address);
    }
}

void minimote_server_stop_tcp(minimote_server *server) {
    server->tcp_running = false;
}

void minimote_server_stop_udp(minimote_server *server) {
    server->udp_running = false;
}

void minimote_server_handle_packet( minimote_server *server,
                                    minimote_packet *packet,
                                    struct sockaddr_in *client) {
    switch (packet->event_type) {
        case LEFT_DOWN:
            minimote_controller_left_down(&server->controller);
            break;
        case LEFT_UP:
            minimote_controller_left_up(&server->controller);
            break;
        case LEFT_CLICK:
            minimote_controller_left_click(&server->controller);
            break;
        case MIDDLE_DOWN:
            minimote_controller_middle_down(&server->controller);
            break;
        case MIDDLE_UP:
            minimote_controller_middle_up(&server->controller);
            break;
        case MIDDLE_CLICK:
            minimote_controller_middle_click(&server->controller);
            break;
        case RIGHT_DOWN:
            minimote_controller_right_down(&server->controller);
            break;
        case RIGHT_UP:
            minimote_controller_right_up(&server->controller);
            break;
        case RIGHT_CLICK:
            minimote_controller_right_click(&server->controller);
            break;
        case MOVE: {
            uint32 payload_value = bytes_to_uint32(packet->payload);
            uint8 mid = (payload_value >> 24) & 0xFF;
            uint16 x = (payload_value >> 12) & 0xFFF;
            uint16 y = payload_value & 0xFFF;
            minimote_controller_move(&server->controller, packet->event_time, mid, x, y);
            break;
        }
        case SCROLL_UP:
            minimote_controller_scroll_up(&server->controller);
            break;
        case SCROLL_DOWN:
            minimote_controller_scroll_down(&server->controller);
            break;
        case TYPE: {
            if (packet->expected_packet_length - MINIMOTE_PACKET_HEADER_SIZE > 1) {
                fprintf(stderr, "Cannot handle yet, key too complex\n");
                break;
            }
            uint8 payload_value = bytes_to_uint8(packet->payload);
            minimote_controller_key_click(&server->controller, payload_value);
            break;
        }
        case KEY_DOWN: {
            uint8 payload_value = bytes_to_uint8(packet->payload);
            minimote_controller_special_key_down(&server->controller, (minimote_special_key_type) payload_value);
            break;
        }
        case KEY_UP: {
            uint8 payload_value = bytes_to_uint8(packet->payload);
            minimote_controller_special_key_up(&server->controller, (minimote_special_key_type) payload_value);
            break;
        }
        case KEY_CLICK: {
            uint8 payload_value = bytes_to_uint8(packet->payload);
            minimote_controller_special_key_click(&server->controller, (minimote_special_key_type) payload_value);
            break;
        }
        case HOTKEY: {
            int payload_length = packet->expected_packet_length - MINIMOTE_PACKET_HEADER_SIZE;

            // TODO: Use a stack push() + pop()
            for (int i = 0; i < payload_length; i++) {
                uint8 key = bytes_to_uint8(&packet->payload[i]);
                minimote_controller_special_key_down(&server->controller, (minimote_special_key_type) key);
                msleep(5);
            }

            for (int i = payload_length - 1; i >= 0; i--) {
                uint8 key = bytes_to_uint8(&packet->payload[i]);
                minimote_controller_special_key_up(&server->controller, (minimote_special_key_type) key);
                if (i != 0)
                    msleep(5);
            }

            break;
        }
        case DISCOVER_REQUEST: {
            printf("Received DISCOVER_REQUEST\n");

            if (!client) {
                fprintf(stderr, "Cannot handle DISCOVER, unknown client\n");
                return;
            }

            if (answer_discover(server, client)) {
                printf("Answered to DISCOVER\n");
            } else {
                fprintf(stderr, "Failed to answer to DISCOVER\n");
            }

            break;
        }
        default:
            fprintf(stderr, "Cannot handle event: %d\n", packet->event_type);
    }
}

bool answer_discover(minimote_server *server, struct sockaddr_in *client) {
    printf("Answering to discover\n");
    char hostname[MAX_HOSTNAME_LENGTH];
    int ok = gethostname(hostname, MAX_HOSTNAME_LENGTH);
    if (ok < 0) {
        fprintf(stderr, "Error resolving hostname\n");
        return false;
    }

    printf("hostname: %s\n", hostname);

    minimote_packet response;
    response.event_type = DISCOVER_RESPONSE;
    response.event_time = current_ms();
    response.expected_packet_length = MINIMOTE_PACKET_HEADER_SIZE + strlen(hostname);
    response.payload = (byte *) hostname;
    response.valid = true;
    printf("Dump of crafted packet\n");
    minimote_packet_dump(&response);

    byte packet_data[64];
    minimote_packet_data(&response, packet_data);

    printf("Build packet of %d bytes\n", response.expected_packet_length);
    printf("Dump of clone of crafted and parsed packet\n");
    minimote_packet response_clone;
    minimote_packet_parse(&response_clone, packet_data, response.expected_packet_length);
    minimote_packet_dump(&response_clone);

    return sendto(server->udp_socket, packet_data, response.expected_packet_length,
            MSG_CONFIRM, (const struct sockaddr *) client, sizeof(*client)) > 0;
}
