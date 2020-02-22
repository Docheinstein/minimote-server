#include "arguments.h"
#include "commons/conf/conf.h"
#include <string.h>
#include <stdlib.h>

#define PARAM_HELP "-h"
#define PARAM_PORT "-p"
#define PARAM_MOUSE_SENSIBILITY "-m"
#define PARAM_SCROLL_SENSIBILITY "-s"
#define PARAM_SCROLL_REVERSE "-R"

arguments arguments_parse(int argc, char **argv) {
    arguments args;
    args.help = false;
    args.port = MINIMOTE_DEFAULT_PORT;
    args.mouse_sensibility = MINIMOTE_DEFAULT_MOUSE_SENSIBILITY;
    args.scroll_boost = MINIMOTE_DEFAULT_SCROLL_SENSIBILITY;
    args.scroll_reverse = MINIMOTE_DEFAULT_SCROLL_REVERSE;

    if (argc <= 1)
        return args;

    for (int i = 1; i < argc; i++) {
        // -p <PORT: int>
        if (!strncmp(PARAM_PORT, argv[i], 2) && i < argc - 1) {
            ++i;
            args.port = (int) strtol(argv[i], NULL, 10);
        }

        // -m <MOUSE_SENSIBILITY: float>
        if (!strncmp(PARAM_MOUSE_SENSIBILITY, argv[i], 2) && i < argc - 1) {
            ++i;
            args.mouse_sensibility = (float) strtof(argv[i], NULL);
        }

        // -s <SCROLL_BOOST: int>
        if (!strncmp(PARAM_SCROLL_SENSIBILITY, argv[i], 2) && i < argc - 1) {
            ++i;
            args.scroll_boost = (int) strtol(argv[i], NULL, 10);
        }

        // -r
        if (!strncmp(PARAM_SCROLL_REVERSE, argv[i], 2)) {
            args.scroll_reverse = true;
        }

        // -h
        if (!strncmp(PARAM_HELP, argv[i], 2)) {
            args.help = true;
        }
    }

    return args;
}
