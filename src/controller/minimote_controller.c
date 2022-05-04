#include "minimote_controller.h"
#include "conf.h"
#include "log/log.h"


static void minimote_controller_scroll(
        minimote_controller *controller,
        void (*scroll_fun)(minimote_display *)
);

minimote_controller_config minimote_controller_config_default() {
    minimote_controller_config cfg;
    cfg.mouse_sensibility = MINIMOTE_DEFAULT_MOUSE_SENSIBILITY;
    cfg.scroll_sensibility = MINIMOTE_DEFAULT_SCROLL_SENSIBILITY;
    cfg.scroll_reverse = MINIMOTE_DEFAULT_SCROLL_REVERSE;
    return cfg;
}

void minimote_controller_init(
        minimote_controller *controller,
        minimote_display *display,
        minimote_controller_config config) {
    controller->display = display;
    controller->config = config;
    controller->last_move.id = 0;
    controller->last_move.time = 0;
    controller->last_move.x = 0;
    controller->last_move.y = 0;
}


void minimote_controller_destroy(minimote_controller *controller) {

}

void minimote_controller_move(minimote_controller *controller,
        uint64 time, uint8 mid, uint16 x, uint16 y) {

    if (time < controller->last_move.time) {
        w("Discarding out of order MOVE packet");
        return;
    }

    if (controller->last_move.time > 0 &&
        controller->last_move.id == mid) {

        // Legal movement, belongs to the same pointer of the last event
        int dx = (int) ((x - controller->last_move.x) * controller->config.mouse_sensibility);
        int dy = (int) ((y - controller->last_move.y) * controller->config.mouse_sensibility);

        minimote_display_move(controller->display, dx, dy);
    }
    else {
        // New pointer down, skip since we cannot compute delta yet
        d("New movement id: %d", mid);
    }

    controller->last_move.time = time;
    controller->last_move.id = mid;
    controller->last_move.x = x;
    controller->last_move.y = y;
}

void minimote_controller_left_down(minimote_controller *controller) {
    minimote_display_left_down(controller->display);
}

void minimote_controller_left_up(minimote_controller *controller) {
    minimote_display_left_up(controller->display);
}

void minimote_controller_left_click(minimote_controller *controller) {
    minimote_display_left_click(controller->display);
}

void minimote_controller_middle_down(minimote_controller *controller) {
    minimote_display_middle_down(controller->display);
}

void minimote_controller_middle_up(minimote_controller *controller) {
    minimote_display_middle_up(controller->display);
}

void minimote_controller_middle_click(minimote_controller *controller) {
    minimote_display_middle_click(controller->display);
}

void minimote_controller_right_down(minimote_controller *controller) {
    minimote_display_right_down(controller->display);
}

void minimote_controller_right_up(minimote_controller *controller) {
    minimote_display_right_up(controller->display);
}

void minimote_controller_right_click(minimote_controller *controller) {
    minimote_display_right_click(controller->display);
}

void minimote_controller_scroll_up(minimote_controller *controller) {
    minimote_controller_scroll(controller,
                               controller->config.scroll_reverse ?
                               minimote_display_scroll_down : minimote_display_scroll_up);
}

void minimote_controller_scroll_down(minimote_controller *controller) {
    minimote_controller_scroll(controller,
                               controller->config.scroll_reverse ?
                               minimote_display_scroll_up : minimote_display_scroll_down);
}

void minimote_controller_write(minimote_controller *controller, uint32 unicode_key) {
    minimote_display_write(controller->display, unicode_key);
}

void minimote_controller_key_down(minimote_controller *controller, minimote_key_type minimote_key) {
    minimote_display_key_down(controller->display, minimote_key);
}

void minimote_controller_key_up(minimote_controller *controller, minimote_key_type minimote_key) {
    minimote_display_key_up(controller->display, minimote_key);
}

void minimote_controller_key_click(minimote_controller *controller, minimote_key_type minimote_key) {
    minimote_display_key_click(controller->display, minimote_key);
}

void minimote_controller_hotkey(minimote_controller *controller,
        minimote_key_type *minimote_keys, int minimote_key_count) {

    // Iterate forward and for each byte press the corresponding key down
    for (int i = 0; i < minimote_key_count; i++) {
        minimote_controller_key_down(controller, minimote_keys[i]);
        msleep(MINIMOTE_HOTKEY_INTER_KEYS_DELAY_MS);
    }

    // Iterate backward and for each byte press the corresponding key up
    for (int i = minimote_key_count - 1; i >= 0; i--) {
        minimote_controller_key_up(controller, minimote_keys[i]);
        if (i > 0)
            msleep(MINIMOTE_HOTKEY_INTER_SCROLLS_DELAY_MS);
    }
}

// ----

void minimote_controller_scroll(
        minimote_controller *controller,
        void (*scroll_fun)(minimote_display *)) {

    for (int i = 0; i < controller->config.scroll_sensibility; i++) {
        scroll_fun(controller->display);
        if (i < controller->config.scroll_sensibility - 1)
            msleep(MINIMOTE_HOTKEY_INTER_SCROLLS_DELAY_MS);
    }
}
