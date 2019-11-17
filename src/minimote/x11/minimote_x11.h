#ifndef MINIMOTE_X11_H
#define MINIMOTE_X11_H

#include <X11/Xlib.h>
#include "adt/hash/hash.h"
#include "commons/globals.h"

#define MODIFIERS_COUNT Mod5MapIndex + 1

typedef struct minimote_x11_t {
    Display *display;
    hash keymap;
    KeyCode modifiers[MODIFIERS_COUNT];
} minimote_x11;

void minimote_x11_init(minimote_x11 *mx);

void minimote_x11_left_down(minimote_x11 *mx);
void minimote_x11_left_up(minimote_x11 *mx);
void minimote_x11_left_click(minimote_x11 *mx);

void minimote_x11_middle_down(minimote_x11 *mx);
void minimote_x11_middle_up(minimote_x11 *mx);
void minimote_x11_middle_click(minimote_x11 *mx);

void minimote_x11_right_down(minimote_x11 *mx);
void minimote_x11_right_up(minimote_x11 *mx);
void minimote_x11_right_click(minimote_x11 *mx);

void minimote_x11_move(minimote_x11 *mx, int dx, int dy);

void minimote_x11_scroll_up(minimote_x11 *mx);
void minimote_x11_scroll_down(minimote_x11 *mx);

void minimote_x11_key_click(minimote_x11 *mx, uint32 unicode_key);

#endif // MINIMOTE_X11_H
