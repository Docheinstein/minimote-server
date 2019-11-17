#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "commons/globals.h"
#include "commons/utils/byte_utils.h"
#include "minimote_server.h"
#include "minimote/packet/type/minimote_packet_type.h"
#include "minimote/packet/minimote_packet.h"

static void minimote_server_handle_packet(  minimote_controller *controller,
                                            minimote_packet *packet);

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

        minimote_packet packet;
        minimote_packet_parse(&packet, buffer, received_bufflen);
        minimote_packet_dump(&packet);

        minimote_server_handle_packet(&arg->server->controller, &packet);
    }

    close(arg->connectionfd);
    free(arg);

    return NULL;
}

void minimote_server_init(minimote_server *server,
                          int tcp_port,
                          int udp_port) {
    server->tcp_running = 0;
    server->udp_running = 0;
    server->udp_port = udp_port;
    server->tcp_port = tcp_port;
    minimote_controller_init(&server->controller);
}

void minimote_server_start_tcp(minimote_server *server) {
    printf("Starting TCP server on port %d\n", server->tcp_port);
    server->tcp_running = true;

    struct sockaddr_in server_address;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        fprintf(stderr, "Socket creation failed: %s\n", strerror(errno));
        exit(-1);
    }
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
               &opt, sizeof(opt));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(server->udp_port);

    if (bind(sockfd, (struct sockaddr *)& server_address, sizeof(server_address)) < 0) {
        fprintf(stderr, "Socket bind failed: %s\n", strerror(errno));
        exit(-1);
    }

    if (listen(sockfd, 1) != 0) {
        fprintf(stderr, "Socket listening failed: %s\n", strerror(errno));
        exit(-1);
    }

    while (server->tcp_running) {
        // Handle clients
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);

        printf("Waiting for TCP connections...\n");

        int connectionfd = accept(sockfd, (struct sockaddr *) &client_address, &client_address_len);
        char connection_ip[INET_ADDRSTRLEN];

        printf("Received TCP connection from %s:%d (fd = %d)\n",
               inet_ntop(AF_INET, &(client_address.sin_addr), connection_ip, INET_ADDRSTRLEN),
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

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        fprintf(stderr, "Socket creation failed: %s\n", strerror(errno));
        exit(-1);
    }
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
               &opt, sizeof(opt));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(server->udp_port);

    if (bind(sockfd, (struct sockaddr *)& server_address, sizeof(server_address)) < 0) {
        fprintf(stderr, "Socket bind failed: %s\n", strerror(errno));
        exit(-1);
    }

    const int BUFFER_SIZE = 20;

    byte buffer[BUFFER_SIZE];

    while (server->udp_running) {
        memset(buffer, 0, BUFFER_SIZE);

        printf("Waiting for UDP messages...\n");

        int received_bufflen = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);

        if (received_bufflen <= 0) {
            fprintf(stderr, "Error in recvfrom\n");
            exit(-1);
        }

        printf("Received UDP message (%d bytes)\n", received_bufflen);

        minimote_packet packet;
        minimote_packet_parse(&packet, buffer, received_bufflen);
        minimote_packet_dump(&packet);

        minimote_server_handle_packet(&server->controller, &packet);
    }
}

void minimote_server_stop_tcp(minimote_server *server) {
    server->tcp_running = false;
}

void minimote_server_stop_udp(minimote_server *server) {
    server->udp_running = false;
}


void minimote_server_handle_packet( minimote_controller *controller,
                                    minimote_packet *packet) {
    switch (packet->event_type) {
        case LEFT_DOWN:
            minimote_controller_left_down(controller);
            break;
        case LEFT_UP:
            minimote_controller_left_up(controller);
            break;
        case LEFT_CLICK:
            minimote_controller_left_click(controller);
            break;
        case MIDDLE_DOWN:
            minimote_controller_middle_down(controller);
            break;
        case MIDDLE_UP:
            minimote_controller_middle_up(controller);
            break;
        case MIDDLE_CLICK:
            minimote_controller_middle_click(controller);
            break;
        case RIGHT_DOWN:
            minimote_controller_right_down(controller);
            break;
        case RIGHT_UP:
            minimote_controller_right_up(controller);
            break;
        case RIGHT_CLICK:
            minimote_controller_right_click(controller);
            break;
        case MOVE: {
            uint32 payload_value = bytes_to_uint32(packet->payload);
            uint8 mid = (payload_value >> 24) & 0xFF;
            uint16 x = (payload_value >> 12) & 0xFFF;
            uint16 y = payload_value & 0xFFF;
            minimote_controller_move(controller, packet->event_time, mid, x, y);
            break;
        }
        case SCROLL_UP:
            minimote_controller_scroll_up(controller);
            break;
        case SCROLL_DOWN:
            minimote_controller_scroll_down(controller);
            break;
        case KEY_CLICK: {
            uint8 payload_value = bytes_to_uint8(packet->payload);
            minimote_controller_key_click(controller, payload_value);
            break;
        }
        default:
            fprintf(stderr, "Cannot handle event: %d\n", packet->event_type);
    }
}