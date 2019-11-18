#ifndef MINIMOTE_CONTROLLER_H
#define MINIMOTE_CONTROLLER_H

#include <minimote/special_keys/type/minimote_special_key_type.h>
#include "commons/globals.h"
#include "minimote/x11/minimote_x11.h"

typedef struct minimote_controller_t {
    uint64 last_move_time;
    uint8 last_move_id;
    uint16 last_move_x;
    uint16 last_move_y;
    pthread_mutex_t x11_mutex;
    minimote_x11 x11;

} minimote_controller;

void minimote_controller_init(minimote_controller *controller);

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

void minimote_controller_key_click(minimote_controller *controller, uint32 unicode_key);
void minimote_controller_special_key_down(minimote_controller *controller, minimote_special_key_type special_key);
void minimote_controller_special_key_up(minimote_controller *controller, minimote_special_key_type special_key);
void minimote_controller_special_key_click(minimote_controller *controller, minimote_special_key_type special_key);

#endif // MINIMOTE_CONTROLLER_H
