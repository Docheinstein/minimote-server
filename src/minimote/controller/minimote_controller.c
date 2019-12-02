#include "minimote_controller.h"
#include <stdio.h>
#include <logging/logging.h>
#include <commons/conf/conf.h>
#include <commons/utils/time_utils.h>
#include "minimote/x11/minimote_x11.h"

#define HOTKEY_INTER_SCROLLS_TIMEOUT_MS 20
#define HOTKEY_INTER_KEYS_TIMEOUT_MS 5

static void minimote_controller_merge_config(
        minimote_controller_config *dst,
        const minimote_controller_config *src);

static void minimote_controller_scroll(
        minimote_controller *controller,
        void (*scroll_fun)(minimote_x11 *)
);

minimote_controller_config minimote_controller_config_default() {
    minimote_controller_config cfg;
    cfg.mouse_sensisibilty = MINIMOTE_DEFAULT_MOUSE_SENSIBILITY;
    cfg.scroll_boost = MINIMOTE_DEFAULT_SCROLL_SENSIBILITY;
    cfg.scroll_reverse = MINIMOTE_DEFAULT_SCROLL_REVERSE;
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
}

void minimote_controller_move(minimote_controller *controller,
        uint64 time, uint8 mid, uint16 x, uint16 y) {

    if (time < controller->last_move_time) {
        w("Discarding out of order MOVE packet");
        return;
    }

    v("Handling movement (MID: %d, X: %d, Y: %d)\n", mid, x, y);

    if (controller->last_move_time > 0 &&
        controller->last_move_id == mid) {

        // Legal movement, belongs to the same pointer of the last event
        int dx = (int) ((x - controller->last_move_x) * controller->config.mouse_sensisibilty);
        int dy = (int) ((y - controller->last_move_y) * controller->config.mouse_sensisibilty);

        minimote_x11_move(&X11, dx, dy);
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
    minimote_x11_left_down(&X11);
}

void minimote_controller_left_up(minimote_controller *controller) {
    minimote_x11_left_up(&X11);
}

void minimote_controller_left_click(minimote_controller *controller) {
    minimote_x11_left_click(&X11);
}

void minimote_controller_middle_down(minimote_controller *controller) {
    minimote_x11_middle_down(&X11);
}

void minimote_controller_middle_up(minimote_controller *controller) {
    minimote_x11_middle_up(&X11);
}

void minimote_controller_middle_click(minimote_controller *controller) {
    minimote_x11_middle_click(&X11);
}

void minimote_controller_right_down(minimote_controller *controller) {
    minimote_x11_right_down(&X11);
}

void minimote_controller_right_up(minimote_controller *controller) {
    minimote_x11_right_up(&X11);
}

void minimote_controller_right_click(minimote_controller *controller) {
    minimote_x11_right_click(&X11);
}

void minimote_controller_scroll_up(minimote_controller *controller) {
    minimote_controller_scroll(controller,
                               controller->config.scroll_reverse ?
                               minimote_x11_scroll_down : minimote_x11_scroll_up);
}

void minimote_controller_scroll_down(minimote_controller *controller) {
    minimote_controller_scroll(controller,
                               controller->config.scroll_reverse ?
                               minimote_x11_scroll_up : minimote_x11_scroll_down);
}

void minimote_controller_write(minimote_controller *controller, uint32 unicode_key) {
    minimote_x11_write(&X11, unicode_key);
}

void minimote_controller_key_down(minimote_controller *controller, minimote_key_type minimote_key) {
    minimote_x11_key_down(&X11, minimote_key);
}

void minimote_controller_key_up(minimote_controller *controller, minimote_key_type minimote_key) {
    minimote_x11_key_up(&X11, minimote_key);
}

void minimote_controller_key_click(minimote_controller *controller, minimote_key_type minimote_key) {
    minimote_x11_key_click(&X11, minimote_key);
}

void minimote_controller_hotkey(minimote_controller *controller,
        minimote_key_type *minimote_keys, int minimote_key_count) {

    // Iterate forward and for each byte press the corresponding key down
    for (int i = 0; i < minimote_key_count; i++) {
        minimote_controller_key_down(controller, minimote_keys[i]);
        msleep(HOTKEY_INTER_KEYS_TIMEOUT_MS);
    }

    // Iterate backward and for each byte press the corresponding key up
    for (int i = minimote_key_count - 1; i >= 0; i--) {
        minimote_controller_key_up(controller, minimote_keys[i]);
        if (i > 0)
            msleep(HOTKEY_INTER_KEYS_TIMEOUT_MS);
    }
}

// ----

void minimote_controller_merge_config(  minimote_controller_config *dst,
                                        const minimote_controller_config *src) {
    if (!dst || !src)
        return;
    dst->mouse_sensisibilty = src->mouse_sensisibilty;
    dst->scroll_boost = src->scroll_boost;
    dst->scroll_reverse = src->scroll_reverse;
}

void minimote_controller_scroll(
        minimote_controller *controller,
        void (*scroll_fun)(minimote_x11 *)) {

    for (int i = 0; i < controller->config.scroll_boost; i++) {
        scroll_fun(&X11);
        if (i < controller->config.scroll_boost - 1)
            msleep(HOTKEY_INTER_SCROLLS_TIMEOUT_MS);
    }
}
