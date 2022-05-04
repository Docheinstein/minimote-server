#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "args/args.h"
#include "commons/utils/linux_utils.h"
#include "commons/utils/net_utils.h"
#include "conf.h"
#include "controller/minimote_controller.h"
#include "display/minimote_display.h"
#include "log/log.h"
#include "server/minimote_server.h"

static void sigint_handler(int sig) {
    minimote_server_is_running = false;
}


static void setup_sigint_handler() {
    struct sigaction sa;
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) < 0) {
        w("Failed to attach to SIGINT event");
    }

    signal(SIGINT, sigint_handler);
}
int main(int argc, char **argv) {
    i("======== MINIMOTE =========");
    i("Version: " MINIMOTE_SERVER_VERSION);
#ifdef X11
    d("Compiled with X11 support");
#endif
#ifdef WAYLAND
    d("Compiled with Wayland support");
#endif
    i("===========================");

    d("Parsing arguments....");

    args args;

    args_init(&args);
    args_parse(&args, argc, argv);

    log_verbose_enable(args.verbose);

    char *s = args_to_string(&args);
    d("Arguments:\n"
      "------------------\n"
      "%s\n"
      "------------------", s);
    free(s);

    minimote_display_config display_config;
    display_config.display_server = LINUX_DISPLAY_SERVER_UNKNOWN;

#if defined(X11) && defined(WAYLAND)
    if (args.x11 && args.wayland) {
        e("Cannot use both x11 and wayland, please specify whether use x11 (-x) or wayland (-w)");
        exit(EXIT_FAILURE);
    }

    if (args.x11)
        display_config.display_server = LINUX_DISPLAY_SERVER_X11;
    if (args.wayland)
        display_config.display_server = LINUX_DISPLAY_SERVER_WAYLAND;

    if (display_config.display_server == LINUX_DISPLAY_SERVER_UNKNOWN) {
        e("Please specify whether use x11 (-x) or wayland (-w)");
        exit(EXIT_FAILURE);
    }
#elif defined(X11)
    display_config.display_server = LINUX_DISPLAY_SERVER_X11;
#elif defined(WAYLAND)
    display_config.display_server = LINUX_DISPLAY_SERVER_WAYLAND;
#endif

    minimote_display display;
    minimote_display_init(&display, display_config);

    minimote_controller_config controller_config;
    controller_config.mouse_sensibility = args.mouse_sensibility;
    controller_config.scroll_sensibility = args.scroll_sensibility;
    controller_config.scroll_reverse = args.scroll_reverse;

    minimote_server_config server_config;
    server_config.tcp_port = server_config.udp_port = args.port;

    minimote_server server;
    minimote_server_init(&server, &display, controller_config, server_config);

    setup_sigint_handler();

    minimote_server_start(&server);

    // We come here after CTRL+C

    minimote_server_destroy(&server);
    minimote_display_destroy(&display);

    args_destroy(&args);

    return 0;
}
