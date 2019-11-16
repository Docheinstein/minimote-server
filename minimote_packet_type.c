#include "minimote_packet_type.h"
#include "string.h"

const char * minimote_packet_type_to_string(minimote_packet_type type) {
    switch (type) {
        case LEFT_DOWN:
            return "LEFT_DOWN";
        case LEFT_UP:
            return "LEFT_UP";
        case LEFT_CLICK:
            return "LEFT_CLICK";
        case MIDDLE_DOWN:
            return "MIDDLE_DOWN";
        case MIDDLE_UP:
            return "MIDDLE_UP";
        case MIDDLE_CLICK:
            return "MIDDLE_CLICK";
        case RIGHT_DOWN:
            return "RIGHT_DOWN";
        case RIGHT_UP:
            return "RIGHT_UP";
        case RIGHT_CLICK:
            return "RIGHT_CLICK";
        case MOVE:
            return "MOVE";
        case SCROLL_UP:
            return "SCROLL_UP";
        case SCROLL_DOWN:
            return "SCROLL_DOWN";
        case KEY_DOWN:
            return "KEY_DOWN";
        case KEY_UP:
            return "KEY_UP";
        case KEY_CLICK:
            return "KEY_CLICK";
        case KEYS:
            return "KEYS";
        case DISCOVER:
            return "DISCOVER";
    }
}

