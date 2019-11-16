#ifndef MINIMOTE_X11_H
#define MINIMOTE_X11_H

#include <X11/Xlib.h>

typedef struct minimote_x11_t {
    Display *display;

} minimote_x11;

void minimote_x11_init(minimote_x11 *mx);

void minimote_x11_left_down(minimote_x11 *mx);
void minimote_x11_left_up(minimote_x11 *mx);

void minimote_x11_move(minimote_x11 *mx, int dx, int dy);

#endif // MINIMOTE_X11_H
