#ifndef MINIMOTE_PACKET_TYPE_H
#define MINIMOTE_PACKET_TYPE_H

typedef enum minimote_packet_type_t {
    LEFT_DOWN       = 0x00,
    LEFT_UP         = 0x01,
    LEFT_CLICK      = 0x02,
    MIDDLE_DOWN     = 0x03,
    MIDDLE_UP       = 0x04,
    MIDDLE_CLICK    = 0x05,
    RIGHT_DOWN      = 0x06,
    RIGHT_UP        = 0x07,
    RIGHT_CLICK     = 0x08,
    MOVE            = 0x09,
    SCROLL_UP       = 0x0A,
    SCROLL_DOWN     = 0x0B,
    KEY_DOWN        = 0x0C,
    KEY_UP          = 0x0D,
    KEY_CLICK       = 0x0E,
    KEYS            = 0x0F,

    DISCOVER        = 0xFF
} minimote_packet_type;

const char * minimote_packet_type_to_string(minimote_packet_type type);

#endif // MINIMOTE_PACKET_TYPE_H
