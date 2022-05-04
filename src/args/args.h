#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>

typedef struct args {
#if defined(X11) && defined(WAYLAND)
    bool x11;
    bool wayland;
#endif
    bool verbose;
    int port;
    float mouse_sensibility;
    int scroll_sensibility;
    bool scroll_reverse;
} args;


void args_init(args *args);
void args_parse(args *args, int argc, char **argv);
void args_destroy(args *args);

char *args_to_string(const args *args);

#endif // ARGS_H