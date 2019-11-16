#ifndef MINIMOTE_CONTROLLER_H
#define MINIMOTE_CONTROLLER_H

#include "minimote_defs.h"
#include "minimote_x11.h"

typedef struct minimote_controller_t {
    uint64 last_move_time;
    uint8 last_move_id;
    uint16 last_move_x;
    uint16 last_move_y;
    minimote_x11 x11;

} minimote_controller;

void minimote_controller_init(minimote_controller *controller);

void minimote_controller_left_down(minimote_controller *controller);
void minimote_controller_left_up(minimote_controller *controller);
void minimote_controller_move(minimote_controller *controller,
        uint64 time, uint8 mid, uint16 x, uint16 y);

#endif // MINIMOTE_CONTROLLER_H
