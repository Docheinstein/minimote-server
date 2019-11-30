#include "minimote_controller.h"
#include <stdio.h>
#include <logging/logging.h>
#include <commons/conf/conf.h>

static void minimote_controller_merge_config(
        minimote_controller_config *dst,
        const minimote_controller_config *src);

minimote_controller_config minimote_controller_config_default() {
    minimote_controller_config cfg;
    cfg.mouse_sensisibilty = MINIMOTE_DEFAULT_MOUSE_SENSIBILITY;
    return cfg;
}

void minimote_controller_init(
        minimote_controller *controller,
        minimote_controller_config config) {
    minimote_controller_merge_config(&controller->config, &config);
    controller->last_move_time = 0;
    controller->last_move_id = 0;
    controller->last_move_x = 0;
    controller->last_move_y = 0;
    minimote_x11_init(&controller->x11);
}

void minimote_controller_move(minimote_controller *controller,
        uint64 time, uint8 mid, uint16 x, uint16 y) {

    if (time < controller->last_move_time) {
        w("Discarding out of order MOVE packet");
        return;
    }

    d("Handling movement (MID: %d, X: %d, Y: %d)\n", mid, x, y);

    if (controller->last_move_time > 0 &&
        controller->last_move_id == mid) {

        // Legal movement, belongs to the same pointer of the last event
        int dx = (int) ((x - controller->last_move_x) * controller->config.mouse_sensisibilty);
        int dy = (int) ((y - controller->last_move_y) * controller->config.mouse_sensisibilty);

        minimote_x11_move(&controller->x11, dx, dy);
    }
    else {
        // New pointer down, skip since we cannot compute delta yet
        d("New movement id: %d, doing nothing", mid);
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

void minimote_controller_write(minimote_controller *controller, uint32 unicode_key) {
    minimote_x11_write(&controller->x11, unicode_key);
}

void minimote_controller_key_down(minimote_controller *controller, minimote_key_type minimote_key) {
    minimote_x11_key_down(&controller->x11, minimote_key);
}

void minimote_controller_key_up(minimote_controller *controller, minimote_key_type minimote_key) {
    minimote_x11_key_up(&controller->x11, minimote_key);
}

void minimote_controller_key_click(minimote_controller *controller, minimote_key_type minimote_key) {
    minimote_x11_key_click(&controller->x11, minimote_key);
}

// ----

void minimote_controller_merge_config(  minimote_controller_config *dst,
                                        const minimote_controller_config *src) {
    if (!dst || !src)
        return;
    dst->mouse_sensisibilty = src->mouse_sensisibilty;
}