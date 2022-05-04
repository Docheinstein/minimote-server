#include "args.h"
#include <argp.h>
#include <stdlib.h>
#include "commons/utils/str_utils.h"
#include "conf.h"
#include "log/log.h"

const char *argp_program_version = MINIMOTE_SERVER_VERSION;
static const char *argp_args_doc = "";
static const char *argp_doc = "Allow remote keyboard and mouse control of a Linux host";

typedef enum minimote_option {
#if defined(X11) && defined(WAYLAND)
    OPTION_X11 = 'x',
    OPTION_WAYLAND = 'w',
#endif
    OPTION_VERBOSE = 'v',
    OPTION_PORT = 'p',
    OPTION_MOUSE_SENSIBILITY = 'm',
    OPTION_SCROLL_SENSIBILITY = 's',
    OPTION_SCROLL_REVERSE = 'S',
} minimote_option;

static struct argp_option options[] = {
#if defined(X11) && defined(WAYLAND)
  { "x11", OPTION_X11, NULL, 0,
        "Use X11 as display server" },
  { "wayland", OPTION_WAYLAND, NULL, 0,
        "Use Wayland as display server" },
#endif
  { "verbose", OPTION_VERBOSE, NULL, 0,
        "Print more information" },
  { "port", OPTION_PORT, "PORT", 0,
        "Port (default: 50500)" },
  { "mouse-sensibility", OPTION_MOUSE_SENSIBILITY, "SENSIBILITY", 0,
        "Mouse sensibility (default: 1.5)" },
  { "scroll-sensibility", OPTION_SCROLL_SENSIBILITY, "SENSIBILITY", 0,
        "Scroll sensibility (default: 1)" },
  { "scroll-reverse", OPTION_SCROLL_REVERSE, NULL, 0,
        "Reverse scroll direction" },
  { NULL }
};

static error_t parse_option(int key, char *arg, struct argp_state *state) {
    args *args = state->input;

    switch (key) {
#if defined(X11) && defined(WAYLAND)
    case OPTION_X11:
        args->x11 = true;
        break;
    case OPTION_WAYLAND:
        args->wayland = true;
        break;
#endif
    case OPTION_VERBOSE:
        args->verbose = true;
        break;
    case OPTION_PORT:
        args->port = (int) strtol(arg, NULL, 10);
        break;
    case OPTION_MOUSE_SENSIBILITY:
        args->mouse_sensibility = (float) strtof(arg, NULL);
        break;
    case OPTION_SCROLL_SENSIBILITY:
        args->scroll_sensibility = (int) strtol(arg, NULL, 10);
        break;
    case OPTION_SCROLL_REVERSE:
        args->scroll_reverse = true;
        break;
    case ARGP_KEY_ARG:
        e("Unexpected argument '%s'", arg);
        argp_usage(state);
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }

    return 0;
}


void args_init(args *args) {
#if defined(X11) && defined(WAYLAND)
    args->x11 = 0;
    args->wayland = 0;
#endif

#if LOG_DEBUG
    args->verbose = true;
#else
    args->verbose = false;
#endif

    args->port = MINIMOTE_DEFAULT_PORT;
    args->mouse_sensibility = MINIMOTE_DEFAULT_MOUSE_SENSIBILITY;
    args->scroll_sensibility = MINIMOTE_DEFAULT_SCROLL_SENSIBILITY;
    args->scroll_reverse = MINIMOTE_DEFAULT_SCROLL_REVERSE;
}

void args_parse(args *args, int argc, char **argv) {
    struct argp argp = {options, parse_option, argp_args_doc, argp_doc};
    argp_parse(&argp, argc, argv, 0, 0, args);
}

void args_destroy(args *args) {

}

char *args_to_string(const args *args) {
    return strmake(
#if defined(X11) && defined(WAYLAND)
        "x11 = %s\n"
        "wayland = %s\n"
#endif
        "verbose = %s\n"
        "port = %d\n"
        "mouse_sensibility = %.2f\n"
        "scroll_sensibility = %d\n"
        "scroll_reverse = %s",
#if defined(X11) && defined(WAYLAND)
        booltostr(args->x11),
        booltostr(args->wayland),
#endif
        booltostr(args->verbose),
        args->port,
        args->mouse_sensibility,
        args->scroll_sensibility,
        booltostr(args->scroll_reverse)
    );
}
