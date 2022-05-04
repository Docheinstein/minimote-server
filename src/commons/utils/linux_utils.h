#ifndef LINUX_UTILS_H
#define LINUX_UTILS_H

typedef enum linux_display_server {
    LINUX_DISPLAY_SERVER_WAYLAND,
    LINUX_DISPLAY_SERVER_X11,
    LINUX_DISPLAY_SERVER_UNKNOWN
} linux_display_server;

const char * linux_display_server_to_string(linux_display_server display_server);


#endif // LINUX_UTILS_H