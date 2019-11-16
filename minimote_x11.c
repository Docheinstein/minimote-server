#include "minimote_x11.h"
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>

void minimote_x11_init(minimote_x11 *mx) {
    mx->display = XOpenDisplay(NULL);

    if (!mx->display) {
        fprintf(stderr, "Unable to open display\n");
        exit(-1);
    }

    int event_basep, error_basep, majorp, minorp;
    if (XTestQueryExtension(mx->display, &event_basep, &error_basep, &majorp, &minorp) != True) {
        fprintf(stderr, "Environment does not support x11 TEST extension, please install X11tst\n");
        exit(-1);
    }
}

void minimote_x11_move(minimote_x11 *mx, int dx, int dy) {
    printf("Moving by (dx: %d, y: %d)\n", dx, dy);
    XTestFakeRelativeMotionEvent(mx->display, dx, dy, 0);
    XFlush(mx->display);
}

void minimote_x11_left_down(minimote_x11 *mx) {
    XTestFakeButtonEvent(mx->display, Button1, True, 0);
    XFlush(mx->display);
}

void minimote_x11_left_up(minimote_x11 *mx) {
    XTestFakeButtonEvent(mx->display, Button1, False, 0);
    XFlush(mx->display);
}
