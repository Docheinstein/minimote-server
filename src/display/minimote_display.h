#ifndef MINIMOTE_DISPLAY_H
#define MINIMOTE_DISPLAY_H

#include "commons/containers/hash/hash.h"
#include "commons/types.h"
#include "commons/utils/linux_utils.h"
#include "commons/utils/xkb_utils.h"
#include "keys/minimote_key_type.h"

typedef struct minimote_display_config {
    linux_display_server display_server;
} minimote_display_config;

typedef struct minimote_display {
    minimote_display_config config;

    int uinput_fd;

    struct xkb_context *context;
    struct xkb_keymap *keymap;

    xkb_keysyms_mapping keysyms;
    xkb_modifiers_mapping modifiers;
} minimote_display;

void minimote_display_init(minimote_display *display, minimote_display_config config);
void minimote_display_destroy(minimote_display *display);

void minimote_display_left_down(minimote_display *display);
void minimote_display_left_up(minimote_display *display);
void minimote_display_left_click(minimote_display *display);

void minimote_display_middle_down(minimote_display *display);
void minimote_display_middle_up(minimote_display *display);
void minimote_display_middle_click(minimote_display *display);

void minimote_display_right_down(minimote_display *display);
void minimote_display_right_up(minimote_display *display);
void minimote_display_right_click(minimote_display *display);

void minimote_display_move(minimote_display *display, int dx, int dy);

void minimote_display_scroll_up(minimote_display *display);
void minimote_display_scroll_down(minimote_display *display);

void minimote_display_write(minimote_display *display, uint32 unicode_key);
void minimote_display_key_down(minimote_display *display, minimote_key_type key);
void minimote_display_key_up(minimote_display *display, minimote_key_type key);
void minimote_display_key_click(minimote_display *display, minimote_key_type key);


#endif // MINIMOTE_DISPLAY_H