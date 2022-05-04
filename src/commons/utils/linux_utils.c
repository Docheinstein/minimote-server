#include "linux_utils.h"
#include <stdbool.h>
#include <stdlib.h>
#include "log/log.h"
#include "str_utils.h"


const char * linux_display_server_to_string(linux_display_server display_server) {
    switch (display_server) {
        case LINUX_DISPLAY_SERVER_WAYLAND:
            return "Wayland";
        case LINUX_DISPLAY_SERVER_X11:
            return "X11";
        default:
            return "Unknown";
    }
}
