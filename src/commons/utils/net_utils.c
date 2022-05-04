#include "net_utils.h"
#include <malloc.h>
#include <strings.h>
#include <unistd.h>
#include "log/log.h"

const char * sockaddr_to_ipv4(struct sockaddr_in addr, char *ip) {
    return inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
}

struct sockaddr_in socket_get_address(int socket) {
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    socklen_t len = sizeof(addr);
    getsockname(socket, (struct sockaddr *) &addr, &len);
    return addr;
}

char *get_hostname() {
    char *hostname = malloc(256 * sizeof(char));
    int ok = gethostname(hostname, HOSTNAME_MAX_LENGTH);
    if (ok < 0) {
        w("Error resolving hostname");
        free(hostname);
        return NULL;
    }

    return hostname;
}
