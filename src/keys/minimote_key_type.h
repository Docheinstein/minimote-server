#ifndef MINIMOTE_KEY_TYPE_H
#define MINIMOTE_KEY_TYPE_H

typedef enum minimote_key_type {
    UP =            0x00,
    DOWN =          0x01,
    LEFT =          0x02,
    RIGHT =         0x03,
    VOLUME_UP =     0x04,
    VOLUME_DOWN =   0x05,
    VOLUME_MUTE =   0x06,
    ALT_LEFT =      0x07,
    ALT_RIGHT =     0x08,
    SHIFT_LEFT =    0x09,
    SHIFT_RIGHT =   0x0A,
    CTRL_LEFT =     0x0B,
    CTRL_RIGHT =    0x0C,
    META_LEFT =     0x0D,
    META_RIGHT =    0x0E,
    ALT_GR =        0x0F,
    CAPS_LOCK =     0x10,
    ESC =           0x11,
    TAB =           0x12,
    SPACE =         0x13,
    ENTER =         0x14,
    BACKSPACE =     0x15,
    CANC =          0x16,
    PRINT =         0x17,
    F1 =            0x18,
    F2 =            0x19,
    F3 =            0x1A,
    F4 =            0x1B,
    F5 =            0x1C,
    F6 =            0x1D,
    F7 =            0x1E,
    F8 =            0x1F,
    F9 =            0x20,
    F10 =           0x21,
    F11 =           0x22,
    F12 =           0x23,
    ZERO =          0x24,
    ONE =           0x25,
    TWO =           0x26,
    THREE =         0x27,
    FOUR =          0x28,
    FIVE =          0x29,
    SIX =           0x2A,
    SEVEN =         0x2B,
    EIGHT =         0x2C,
    NINE =          0x2D,
    A =             0x2E,
    B =             0x2F,
    C =             0x30,
    D =             0x31,
    E =             0x32,
    F =             0x33,
    G =             0x34,
    H =             0x35,
    I =             0x36,
    J =             0x37,
    K =             0x38,
    L =             0x39,
    M =             0x3A,
    N =             0x3B,
    O =             0x3C,
    P =             0x3D,
    Q =             0x3E,
    R =             0x3F,
    S =             0x40,
    T =             0x41,
    U =             0x42,
    V =             0x43,
    W =             0x44,
    X =             0x45,
    Y =             0x46,
    Z =             0x47,
} minimote_key_type;

const char * minimote_key_type_to_string(minimote_key_type type);


#endif // MINIMOTE_KEY_TYPE_H