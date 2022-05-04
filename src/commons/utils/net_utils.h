#ifndef NET_UTILS_H
#define NET_UTILS_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define HOSTNAME_MAX_LENGTH 256

char *get_hostname();

const char * sockaddr_to_ipv4(struct sockaddr_in, char *ip);

struct sockaddr_in socket_get_address(int socket);

#endif // NET_UTILS_H
