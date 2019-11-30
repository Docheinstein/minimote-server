#include "arguments.h"
#include "commons/conf/conf.h"
#include <string.h>
#include <stdlib.h>

#define PARAMETER_PORT "-p"

arguments arguments_parse(int argc, char **argv) {
    arguments args;
    args.port = MINIMOTE_DEFAULT_PORT;

    if (argc <= 1)
        return args;

    for (int i = 1; i < argc - 1; i++) {
        // -p <PORT>
        if (!strncmp(PARAMETER_PORT, argv[i], 2)) {
            ++i;
            args.port = (int) strtol(argv[i], NULL, 10);
        }
    }

    return args;
}
