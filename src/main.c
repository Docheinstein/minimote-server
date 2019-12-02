#include <stdio.h>
#include "minimote/server/minimote_server.h"
#include "arguments/arguments.h"
#include "logging/logging.h"
#include "commons/conf/conf.h"

#define TESTING 0

#if TESTING
#include "test/test.h"
#endif

const char * HELP =
"NAME\n"
"   Minimote Server\n"
"\n"
"DESCRIPTION\n"
"   Remote control server for linux, that uses libx11 for interact with the X server\n"
"   Allows mouse movement, keyboard writing, keyboard hotkeys, and supports a discovery mechanism\n"
"\n"
"COMMANDS\n"
"   -h \n"
"       Shows the help message.\n"
"\n"
"   -p <PORT>\n"
"       TCP/UDP port to use.\n"
"       Default: 50500\n"
"\n"
"   -m <MOUSE_SENSIBILITY>\n"
"       Sets the mouse sensibility.\n"
"       Default: 1.5\n"
"\n"
"   -s <SCROLL_SENSIBILITY>\n"
"       Sets the scroll sensibility.\n"
"       Default: 1\n"
"\n"
"   -r\n"
"       Whether reverse the scroll direction.\n"
"       Default: false\n"
"\n";

static void init_globals();

int main(int argc, char *argv[]) {
#if TESTING
    tests();
#endif

    // Parse arguments
    i("MinimoteServer version " MINIMOTE_SERVER_VERSION);

    v("Going to parse arguments");
    arguments args = arguments_parse(argc, argv);

    if (args.help) {
        printf("%s", HELP);
        return 0;
    }

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