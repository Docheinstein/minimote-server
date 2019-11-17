#include "minimote_controller.h"
#include <stdio.h>

#define DEFAULT_MOUSE_SENSIBILITY 1.5

void minimote_controller_init(minimote_controller *controller) {
    controller->last_move_time = 0;
    controller->last_move_id = 0;
    controller->last_move_x = 0;
    controller->last_move_y = 0;
    minimote_x11_init(&controller->x11);
}



void minimote_controller_move(minimote_controller *controller,
        uint64 time, uint8 mid, uint16 x, uint16 y) {

    if (time < controller->last_move_time) {
        printf("Discarding out of order MOVE packet\n");
        return;
    }

    printf("Handling movement (MID: %d, X: %d, Y: %d)\n", mid, x, y);

    if (controller->last_move_time > 0 &&
        controller->last_move_id == mid) {
        // Legal movement, belongs to the same pointer of the last event
        int dx = (int) ((x - controller->last_move_x) * DEFAULT_MOUSE_SENSIBILITY);
        int dy = (int) ((y - controller->last_move_y) * DEFAULT_MOUSE_SENSIBILITY);

        minimote_x11_move(&controller->x11, dx, dy);
    }
    else {
        // New pointer down, skip since we cannot compute delta yet
        printf("New movement id: %d, doing nothing\n", mid);
    }

    controller->last_move_time = time;
    controller->last_move_id = mid;
    controller->last_move_x = x;
    controller->last_move_y = y;
}

void minimote_controller_left_down(minimote_controller *controller) {
    minimote_x11_left_down(&controller->x11);
}

void minimote_controller_left_up(minimote_controller *controller) {
    minimote_x11_left_up(&controller->x11);
}

void minimote_controller_left_click(minimote_controller *controller) {
    minimote_x11_left_click(&controller->x11);
}

void minimote_controller_middle_down(minimote_controller *controller) {
    minimote_x11_middle_down(&controller->x11);
}

void minimote_controller_middle_up(minimote_controller *controller) {
    minimote_x11_middle_up(&controller->x11);
}

void minimote_controller_middle_click(minimote_controller *controller) {
    minimote_x11_middle_click(&controller->x11);
}

void minimote_controller_right_down(minimote_controller *controller) {
    minimote_x11_right_down(&controller->x11);
}

void minimote_controller_right_up(minimote_controller *controller) {
    minimote_x11_right_up(&controller->x11);
}

void minimote_controller_right_click(minimote_controller *controller) {
    minimote_x11_right_click(&controller->x11);
}

void minimote_controller_scroll_up(minimote_controller *controller) {
    minimote_x11_scroll_up(&controller->x11);
}

void minimote_controller_scroll_down(minimote_controller *controller) {
    minimote_x11_scroll_down(&controller->x11);
}

void minimote_controller_key_click(minimote_controller *controller, uint32 unicode_key) {
    printf("Handling key %#04x\n", unicode_key);
    minimote_x11_key_click(&controller->x11, unicode_key);
}
