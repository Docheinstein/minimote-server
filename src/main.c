#include <stdio.h>
#include "minimote/server/minimote_server.h"
#include "arguments/arguments.h"
#include "logging/logging.h"
#include "commons/conf/conf.h"

static void init_globals();

int main(int argc, char *argv[]) {
    // Parse arguments
    i("MinimoteServer version " MINIMOTE_SERVER_VERSION);

    d("Going to parse arguments");
    arguments args = arguments_parse(argc, argv);

    i("Initializing minimote server on port %d", args.port);
    init_globals();

    minimote_server server;
    minimote_controller_config controller_config = minimote_controller_config_default();
    controller_config.mouse_sensisibilty = args.mouse_sensibility;
    controller_config.scroll_boost = args.scroll_boost;
    controller_config.scroll_reverse = args.scroll_reverse;

    minimote_server_init(&server, args.port, args.port, controller_config);
    minimote_server_start(&server);
}

void init_globals() {
    minimote_x11_init(&X11);
}