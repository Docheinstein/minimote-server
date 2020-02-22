#include "net_utils.h"

const char * sockaddr_to_ipv4(struct sockaddr_in client, char *ip) {
    return inet_ntop(AF_INET, &(client.sin_addr), ip, INET_ADDRSTRLEN);
}
