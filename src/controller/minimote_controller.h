#ifndef MINIMOTE_CONTROLLER_H
#define MINIMOTE_CONTROLLER_H

#include "commons/types.h"
#include "display/minimote_display.h"
#include "keys/minimote_key_type.h"

typedef struct minimote_controller_config {
    double mouse_sensibility;
    int scroll_sensibility;
    bool scroll_reverse;
    int event_delay;
} minimote_controller_config;

typedef struct minimote_controller {
    minimote_display *display;

    minimote_controller_config config;

    struct {
        uint8 id;
        uint64 time;
        uint16 x;
        uint16 y;
    } last_move;
} minimote_controller;

minimote_controller_config minimote_controller_config_default();

void minimote_controller_init(
        minimote_controller *controller,
        minimote_display *display,
        minimote_controller_config config);

void minimote_controller_destroy(minimote_controller *controller);

void minimote_controller_left_down(minimote_controller *controller);
void minimote_controller_left_up(minimote_controller *controller);
void minimote_controller_left_click(minimote_controller *controller);

void minimote_controller_middle_down(minimote_controller *controller);
void minimote_controller_middle_up(minimote_controller *controller);
void minimote_controller_middle_click(minimote_controller *controller);

void minimote_controller_right_down(minimote_controller *controller);
void minimote_controller_right_up(minimote_controller *controller);
void minimote_controller_right_click(minimote_controller *controller);

void minimote_controller_scroll_up(minimote_controller *controller);
void minimote_controller_scroll_down(minimote_controller *controller);

void minimote_controller_move(minimote_controller *controller,
        uint64 time, uint8 mid, uint16 x, uint16 y);

void minimote_controller_write(
        minimote_controller *controller, uint32 unicode_key);

void minimote_controller_key_down(
        minimote_controller *controller, minimote_key_type minimote_key);
void minimote_controller_key_up(
        minimote_controller *controller, minimote_key_type minimote_key);
void minimote_controller_key_click(
        minimote_controller *controller, minimote_key_type minimote_key);

void minimote_controller_hotkey(minimote_controller *controller,
        minimote_key_type * minimote_keys, int minimote_key_count);

#endif // MINIMOTE_CONTROLLER_H