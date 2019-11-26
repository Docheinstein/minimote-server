#ifndef NET_UTILS_H
#define NET_UTILS_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const char * sockaddr_to_ipv4(struct sockaddr_in, char *ip);

#endif // NET_UTILS_H
