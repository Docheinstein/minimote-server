#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include "commons/globals.h"

typedef struct arguments_t {
    int port;
    float mouse_sensibility;
    int scroll_boost;
    bool scroll_reverse;
} arguments;

arguments arguments_parse(int argc, char *argv[]);

#endif // ARGUMENTS_H
