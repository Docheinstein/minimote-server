#ifndef ARGUMENTS_H
#define ARGUMENTS_H

typedef struct arguments_t {
    int port;
} arguments;

arguments arguments_parse(int argc, char *argv[]);

#endif // ARGUMENTS_H
