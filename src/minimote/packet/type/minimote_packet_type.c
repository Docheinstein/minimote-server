#include "minimote_packet_type.h"

const char * minimote_packet_type_to_string(minimote_packet_type type) {
    switch (type) {
        case NONE:
            return "NONE";
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
        case SCROLL_DOWN:
            return "SCROLL_DOWN";
        case SCROLL_UP:
            return "SCROLL_UP";
        case MOVE:
            return "MOVE";
        case TYPE:
            return "TYPE";
        case KEY_DOWN:
            return "KEY_DOWN";
        case KEY_UP:
            return "KEY_UP";
        case KEY_CLICK:
            return "KEY_CLICK";
        case HOTKEY:
            return "HOTKEY";
        case DISCOVER_REQUEST:
            return "DISCOVER_REQUEST";
        case DISCOVER_RESPONSE:
            return "DISCOVER_RESPONSE";
        default:
            return "<UNKNOWN>";
    }
}

