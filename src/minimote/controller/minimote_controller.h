#ifndef MINIMOTE_CONTROLLER_H
#define MINIMOTE_CONTROLLER_H

#include "minimote/x11/minimote_x11.h"
#include "minimote/keys/minimote_key_type.h"
#include "commons/globals.h"

typedef struct minimote_controller_config_t {
    double mouse_sensisibilty;
} minimote_controller_config;

typedef struct minimote_controller_t {
    minimote_controller_config config;

    uint64 last_move_time;
    uint8 last_move_id;
    uint16 last_move_x;
    uint16 last_move_y;

    minimote_x11 x11;
} minimote_controller;

minimote_controller_config minimote_controller_config_default();

void minimote_controller_init(
        minimote_controller *controller,
        minimote_controller_config config);

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

void minimote_controller_write(minimote_controller *controller, uint32 unicode_key);
void minimote_controller_key_down(minimote_controller *controller, minimote_key_type minimote_key);
void minimote_controller_key_up(minimote_controller *controller, minimote_key_type minimote_key);
void minimote_controller_key_click(minimote_controller *controller, minimote_key_type minimote_key);

#endif // MINIMOTE_CONTROLLER_H
