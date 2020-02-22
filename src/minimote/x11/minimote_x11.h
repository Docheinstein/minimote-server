#ifndef MINIMOTE_X11_H
#define MINIMOTE_X11_H

#include <X11/Xlib.h>
#include "minimote/keys/minimote_key_type.h"
#include "adt/hash/hash.h"
#include "commons/globals.h"

#define MODIFIERS_COUNT Mod5MapIndex + 1


typedef struct minimote_x11_t {
    Display *display;
    hash keymap; // (KeySym *) -> (keystroke *)
    KeyCode modifiers[MODIFIERS_COUNT];
} minimote_x11;

extern minimote_x11 X11;

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

void minimote_x11_write(minimote_x11 *mx, uint32 unicode_key);
void minimote_x11_key_down(minimote_x11 *mx, minimote_key_type special_key);
void minimote_x11_key_up(minimote_x11 *mx, minimote_key_type special_key);
void minimote_x11_key_click(minimote_x11 *mx, minimote_key_type special_key);

#endif // MINIMOTE_X11_H
