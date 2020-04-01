#ifndef MINIMOTE_PACKET_TYPE_H
#define MINIMOTE_PACKET_TYPE_H

typedef enum minimote_packet_type_t {
    NONE                = 0x00,
    LEFT_DOWN           = 0x01,
    LEFT_UP             = 0x02,
    LEFT_CLICK          = 0x03,
    MIDDLE_DOWN         = 0x04,
    MIDDLE_UP           = 0x05,
    MIDDLE_CLICK        = 0x06,
    RIGHT_DOWN          = 0x07,
    RIGHT_UP            = 0x08,
    RIGHT_CLICK         = 0x09,
    SCROLL_DOWN         = 0x0A,
    SCROLL_UP           = 0x0B,
    MOVE                = 0x0C,
    TYPE                = 0x0D,
    KEY_DOWN            = 0x0E,
    KEY_UP              = 0x0F,
    KEY_CLICK           = 0x10,
    HOTKEY              = 0x11,

    PING                = 0xFC,
    PONG                = 0xFD,
    DISCOVER_REQUEST    = 0xFE,
    DISCOVER_RESPONSE   = 0xFF,
} minimote_packet_type;

const char * minimote_packet_type_to_string(minimote_packet_type type);

#endif // MINIMOTE_PACKET_TYPE_H
