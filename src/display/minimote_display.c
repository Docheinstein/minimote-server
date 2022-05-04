#include "minimote_display.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/uinput.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "commons/utils/str_utils.h"
#include "commons/utils/xkb_utils.h"
#include "conf.h"
#include "log/log.h"
#include "xkbcommon/xkbcommon.h"

#define IOCTL_E0(fd, req) do { \
    if (ioctl(fd, req) < 0) { \
        e("ioctl failed: %s", strerror(errno)); \
        exit(1); \
    } \
} while(0)

#define IOCTL_E1(fd, req, val) do { \
    if (ioctl(fd, req, val) < 0) { \
        e("ioctl failed: %s", strerror(errno)); \
        exit(1); \
    } \
} while(0)

#define IOCTL_W0(fd, req) do { \
    if (ioctl(fd, req) < 0) { \
        w("ioctl failed: %s", strerror(errno)); \
    } \
} while(0)

#define IOCTL_W1(fd, req, val) do { \
    if (ioctl(fd, req, val) < 0) { \
        w("ioctl failed: %s", strerror(errno)); \
    } \
} while(0)

#define IOCTL_MACRO_PICKER(_1, _2, _3, NAME, ...) NAME

#define IOCTL_E(...) IOCTL_MACRO_PICKER(__VA_ARGS__, IOCTL_E1, IOCTL_E0)(__VA_ARGS__)
#define IOCTL_W(...) IOCTL_MACRO_PICKER(__VA_ARGS__, IOCTL_W1, IOCTL_W0)(__VA_ARGS__)

#define UINPUT_PRESS 1
#define UINPUT_RELEASE 0



int MINIMOTE_KEY_TO_LINUX_KEY[Z + 1] = {
        KEY_UP,             // UP
        KEY_DOWN,           // DOWN
        KEY_LEFT,           // LEFT
        KEY_RIGHT,          // RIGHT
        KEY_VOLUMEUP,       // VOLUME_UP
        KEY_VOLUMEDOWN,     // VOLUME_DOWN
        KEY_MUTE,           // VOLUME_MUTE
        KEY_LEFTALT,        // ALT_LEFT
        KEY_RIGHTALT,       // ALT_RIGHT
        KEY_LEFTSHIFT,      // SHIFT_LEFT
        KEY_RIGHTSHIFT,     // SHIFT_RIGHT
        KEY_LEFTCTRL,       // CTRL_LEFT
        KEY_RIGHTCTRL,      // CTRL_RIGHT
        KEY_LEFTMETA,       // META_LEFT
        KEY_RIGHTMETA,      // META_RIGHT
        KEY_ISO,            // ALT_GR
        KEY_CAPSLOCK,       // CAPS_LOCK
        KEY_ESC,            // ESC
        KEY_TAB,            // TAB
        KEY_SPACE,          // SPACE
        KEY_ENTER,          // ENTER
        KEY_BACKSPACE,      // BACKSPACE
        KEY_CANCEL,         // CANC
        KEY_PRINT,          // PRINT
        KEY_F1,             // F1
        KEY_F2,             // F2
        KEY_F3,             // F3
        KEY_F4,             // F4
        KEY_F5,             // F5
        KEY_F6,             // F6
        KEY_F7,             // F7
        KEY_F8,             // F8
        KEY_F9,             // F9
        KEY_F10,            // F10
        KEY_F11,            // F11
        KEY_F12,            // F12
        KEY_0,              // ZERO
        KEY_1,              // ONE
        KEY_2,              // TWO
        KEY_3,              // THREE
        KEY_4,              // FOUR
        KEY_5,              // FIVE
        KEY_6,              // SIX
        KEY_7,              // SEVEN
        KEY_8,              // EIGHT
        KEY_9,              // NINE
        KEY_A,              // A
        KEY_B,              // B
        KEY_C,              // C
        KEY_D,              // D
        KEY_E,              // E
        KEY_F,              // F
        KEY_G,              // G
        KEY_H,              // H
        KEY_I,              // I
        KEY_J,              // J
        KEY_K,              // K
        KEY_L,              // L
        KEY_M,              // M
        KEY_N,              // N
        KEY_O,              // O
        KEY_P,              // P
        KEY_Q,              // Q
        KEY_R,              // R
        KEY_S,              // S
        KEY_T,              // T
        KEY_U,              // U
        KEY_V,              // V
        KEY_W,              // W
        KEY_X,              // X
        KEY_Y,              // Y
        KEY_Z,              // Z
};


int MINIMOTE_KEY_TO_XKB_KEY[Z + 1] = {
        XKB_KEY_Up,             // UP
        XKB_KEY_Down,           // DOWN
        XKB_KEY_Left,           // LEFT
        XKB_KEY_Right,          // RIGHT
        XKB_KEY_XF86AudioRaiseVolume,       // VOLUME_UP
        XKB_KEY_XF86AudioLowerVolume,     // VOLUME_DOWN
        XKB_KEY_XF86AudioMute,           // VOLUME_MUTE
        XKB_KEY_Alt_L,        // ALT_LEFT
        XKB_KEY_Alt_R,       // ALT_RIGHT
        XKB_KEY_Shift_L,      // SHIFT_LEFT
        XKB_KEY_Shift_R,     // SHIFT_RIGHT
        XKB_KEY_Control_L,       // CTRL_LEFT
        XKB_KEY_Control_R,      // CTRL_RIGHT
        XKB_KEY_Meta_L,       // META_LEFT
        XKB_KEY_Meta_R,      // META_RIGHT
        XKB_KEY_ISO_Level3_Shift,            // ALT_GR
        XKB_KEY_Caps_Lock,       // CAPS_LOCK
        XKB_KEY_Escape,            // ESC
        XKB_KEY_Tab,            // TAB
        XKB_KEY_space,          // SPACE
        XKB_KEY_Return,          // ENTER
        XKB_KEY_BackSpace,      // BACKSPACE
        XKB_KEY_Cancel,         // CANC
        XKB_KEY_Print,          // PRINT
        XKB_KEY_F1,             // F1
        XKB_KEY_F2,             // F2
        XKB_KEY_F3,             // F3
        XKB_KEY_F4,             // F4
        XKB_KEY_F5,             // F5
        XKB_KEY_F6,             // F6
        XKB_KEY_F7,             // F7
        XKB_KEY_F8,             // F8
        XKB_KEY_F9,             // F9
        XKB_KEY_F10,            // F10
        XKB_KEY_F11,            // F11
        XKB_KEY_F12,            // F12
        XKB_KEY_0,              // ZERO
        XKB_KEY_1,              // ONE
        XKB_KEY_2,              // TWO
        XKB_KEY_3,              // THREE
        XKB_KEY_4,              // FOUR
        XKB_KEY_5,              // FIVE
        XKB_KEY_6,              // SIX
        XKB_KEY_7,              // SEVEN
        XKB_KEY_8,              // EIGHT
        XKB_KEY_9,              // NINE
        XKB_KEY_A,              // A
        XKB_KEY_B,              // B
        XKB_KEY_C,              // C
        XKB_KEY_D,              // D
        XKB_KEY_E,              // E
        XKB_KEY_F,              // F
        XKB_KEY_G,              // G
        XKB_KEY_H,              // H
        XKB_KEY_I,              // I
        XKB_KEY_J,              // J
        XKB_KEY_K,              // K
        XKB_KEY_L,              // L
        XKB_KEY_M,              // M
        XKB_KEY_N,              // N
        XKB_KEY_O,              // O
        XKB_KEY_P,              // P
        XKB_KEY_Q,              // Q
        XKB_KEY_R,              // R
        XKB_KEY_S,              // S
        XKB_KEY_T,              // T
        XKB_KEY_U,              // U
        XKB_KEY_V,              // V
        XKB_KEY_W,              // W
        XKB_KEY_X,              // X
        XKB_KEY_Y,              // Y
        XKB_KEY_Z,              // Z
};

void uinput_emit_event(int fd, int type, int code, int val) {
    struct input_event ie;

    ie.type = type;
    ie.code = code;
    ie.value = val;
    ie.time.tv_sec = 0;
    ie.time.tv_usec = 0;

    if (write(fd, &ie, sizeof(ie)) < 0) {
        w("Failed to write uinput event");
    }
}

void uinput_emit_syn(int fd) {
    uinput_emit_event(fd, EV_SYN, SYN_REPORT, 0);
}

void uinput_emit_event_and_syn(int fd, int type, int code, int val) {
    uinput_emit_event(fd, type, code, val);
    uinput_emit_syn(fd);
}

typedef struct keystroke {
    xkb_keycode_t keycode;
    xkb_level_index_t level;
    struct {
        size_t size;
        xkb_mod_mask_t *modifiers;
    } modifiers;
} keystroke;


static void minimote_display_key_down_by_linux_keycode(minimote_display *display, int keycode);
static void minimote_display_key_up_by_linux_keycode(minimote_display *display, int keycode);
static void minimote_display_key_click_by_linux_keycode(minimote_display *display, int keycode);

static int xkb_keycode_to_linux_keycode(xkb_keycode_t keycode) {
    return (int) keycode - 8;
}

static void keysyms_mapping_print(void *key, void *value, void *arg) {
    xkb_keysym_t *keysym = key;
    xkb_keysyms_mapping_value *keysym_mapping_value = value;
    struct xkb_keymap *keymap = arg;
    char keysym_name[32];
    xkb_keysym_get_name(*keysym, keysym_name, 32);

    char str[128];
    str[0] = '\0';

    for (int i = 0; i < keysym_mapping_value->modifiers.size; i++)
        strappend(str, 128, "%s + ", xkb_mod_get_name(keysym_mapping_value->modifiers.modifiers[i]));

    strappend(str, 128, "Keycode %u", keysym_mapping_value->keycode);

    d("Keysym %u (%s) is mapped to %s", *keysym, keysym_name, str);
}

void minimote_display_init(minimote_display *display, minimote_display_config config) {
    display->config = config;

    // UINPUT

    struct uinput_setup usetup;

    d("Opening /dev/uinput...");

    display->uinput_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (display->uinput_fd < 0) {
        e("Failed to open /dev/uinput: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    d("Setting up uinput...");

    // Keys
    IOCTL_E(display->uinput_fd, UI_SET_EVBIT, EV_KEY);
    for (int key = 0; key < 255; key++)
        IOCTL_E(display->uinput_fd, UI_SET_KEYBIT, key);

    // Mouse buttons
    IOCTL_E(display->uinput_fd, UI_SET_KEYBIT, BTN_LEFT);
    IOCTL_E(display->uinput_fd, UI_SET_KEYBIT, BTN_RIGHT);
    IOCTL_E(display->uinput_fd, UI_SET_KEYBIT, BTN_MIDDLE);

    // Mouse move
    IOCTL_E(display->uinput_fd, UI_SET_EVBIT, EV_REL);
    IOCTL_E(display->uinput_fd, UI_SET_RELBIT, REL_X);
    IOCTL_E(display->uinput_fd, UI_SET_RELBIT, REL_Y);

    // Mouse scroll
    IOCTL_E(display->uinput_fd, UI_SET_RELBIT, REL_WHEEL);

    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234;
    usetup.id.product = 0x5678;
    strcpy(usetup.name, "Minimote Virtual Device");

    IOCTL_E(display->uinput_fd, UI_DEV_SETUP, &usetup);
    IOCTL_E(display->uinput_fd, UI_DEV_CREATE);

    d("Creating XKB context");

    display->context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    if (!display->context) {
        e("Failed to create XKB context");
        exit(EXIT_FAILURE);
    }

    d("Retrieving keymap...");

#ifdef X11
    if (config.display_server == LINUX_DISPLAY_SERVER_X11)
        display->keymap = xkb_get_keymap_x11(display->context);
#endif

#ifdef WAYLAND
    if (config.display_server == LINUX_DISPLAY_SERVER_WAYLAND)
        display->keymap = xkb_get_keymap_wayland(display->context);
#endif

    if (!display->keymap) {
        e("Failed to retrieve keymap");
        exit(EXIT_FAILURE);
    }

    d("Keymap retrieved");

    d("------- KEYSYMS --------");
    display->keysyms = xkb_keymap_get_keysyms_mapping(display->keymap);
    hash_foreach(&display->keysyms, keysyms_mapping_print, display->keymap);

    d("------ MODIFIERS -------");
    display->modifiers = xkb_keymap_get_modifiers_mapping(display->keymap);
    for (xkb_mod_index_t i = 0; i < XKB_MODIFIERS_NUM; i++) {
        d("Modifier %s is mapped to Keycode %u", xkb_mod_get_name(i), display->modifiers[i]);
    }
    d("------------------------");
}

void minimote_display_destroy(minimote_display *display) {
    IOCTL_W(display->uinput_fd, UI_DEV_DESTROY);

    if (close(display->uinput_fd) < 0)
        w("Failed to close /dev/uinput: %s", strerror(errno));

    xkb_keysyms_mapping_destroy(&display->keysyms);
    xkb_modifiers_mapping_destroy(&display->modifiers);

    xkb_keymap_unref(display->keymap);
    xkb_context_unref(display->context);
}

void minimote_display_left_down(minimote_display *display) {
    uinput_emit_event_and_syn(display->uinput_fd, EV_KEY, BTN_LEFT, UINPUT_PRESS);
}

void minimote_display_left_up(minimote_display *display) {
    uinput_emit_event_and_syn(display->uinput_fd, EV_KEY, BTN_LEFT, UINPUT_RELEASE);
}

void minimote_display_left_click(minimote_display *display) {
    uinput_emit_event_and_syn(display->uinput_fd, EV_KEY, BTN_LEFT, UINPUT_PRESS);
    uinput_emit_event_and_syn(display->uinput_fd, EV_KEY, BTN_LEFT, UINPUT_RELEASE);
}

void minimote_display_middle_down(minimote_display *display) {
    uinput_emit_event_and_syn(display->uinput_fd, EV_KEY, BTN_MIDDLE, UINPUT_PRESS);
}

void minimote_display_middle_up(minimote_display *display) {
    uinput_emit_event_and_syn(display->uinput_fd, EV_KEY, BTN_MIDDLE, UINPUT_RELEASE);
}

void minimote_display_middle_click(minimote_display *display) {
    uinput_emit_event_and_syn(display->uinput_fd, EV_KEY, BTN_MIDDLE, UINPUT_PRESS);
    uinput_emit_event_and_syn(display->uinput_fd, EV_KEY, BTN_MIDDLE, UINPUT_RELEASE);
}

void minimote_display_right_down(minimote_display *display) {
    uinput_emit_event_and_syn(display->uinput_fd, EV_KEY, BTN_RIGHT, UINPUT_PRESS);
}

void minimote_display_right_up(minimote_display *display) {
    uinput_emit_event_and_syn(display->uinput_fd, EV_KEY, BTN_RIGHT, UINPUT_RELEASE);
}

void minimote_display_right_click(minimote_display *display) {
    uinput_emit_event_and_syn(display->uinput_fd, EV_KEY, BTN_RIGHT, UINPUT_PRESS);
    uinput_emit_event_and_syn(display->uinput_fd, EV_KEY, BTN_RIGHT, UINPUT_RELEASE);
}

void minimote_display_move(minimote_display *display, int dx, int dy) {
    uinput_emit_event(display->uinput_fd, EV_REL, REL_X, dx);
    uinput_emit_event(display->uinput_fd, EV_REL, REL_Y, dy);
    uinput_emit_syn(display->uinput_fd);
}

void minimote_display_scroll_up(minimote_display *display) {
    uinput_emit_event_and_syn(display->uinput_fd, EV_REL, REL_WHEEL, 1);
}

void minimote_display_scroll_down(minimote_display *display) {
    uinput_emit_event_and_syn(display->uinput_fd, EV_REL, REL_WHEEL, -1);
}

void minimote_display_write(minimote_display *display, uint32 unicode_key) {
    /*
     * In order to write a unicode character we have to lookup the mapping
     * between the keysym (unicode) and its (modifiers + keycode).
     * Then we have to:
     * 1. Press down the modifiers
     * 2. Press down the keycode
     * 3. Press up the keycode
     * 4. Press up the modifiers (backward)
     */
    xkb_keysym_t keysym = xkb_utf32_to_keysym(unicode_key);

    xkb_keysyms_mapping_value *keysym_value = hash_get(&display->keysyms, &keysym);

    if (!keysym_value) {
        w("No mapping is available for Keysym %u", unicode_key);
        return;
    }

#if LOG_DEBUG
    char keysym_name[32];
    xkb_keysym_get_name(keysym, keysym_name, 32);

    d("Writing '%s'", keysym_name);
    d("- unicode = %u", unicode_key);
    d("- keycode = %u", keysym_value->keycode);
    char modifiers_str[64];
    modifiers_str[0] = '\0';
    for (int i = 0; i < keysym_value->modifiers.size; i++) {

        strappend(modifiers_str, 64, "%s (keycode %d)",
                  xkb_mod_get_name(keysym_value->modifiers.modifiers[i]),
                  display->modifiers[keysym_value->modifiers.modifiers[i]]);
        if (i < keysym_value->modifiers.size - 1)
            strappend(modifiers_str, 64, " %s ", "+");
    }
    d("- modifiers = %s", modifiers_str);
#endif

    // 1. Press down the modifiers
    for (size_t i = 0; i < keysym_value->modifiers.size; i++) {
        minimote_display_key_down_by_linux_keycode(
                display,
                xkb_keycode_to_linux_keycode(
                        display->modifiers[keysym_value->modifiers.modifiers[i]]));
        msleep(MINIMOTE_HOTKEY_INTER_KEYS_DELAY_MS);
    }

    // 2. Press down the keycode
    minimote_display_key_down_by_linux_keycode(
            display,
            xkb_keycode_to_linux_keycode(keysym_value->keycode));
    msleep(MINIMOTE_HOTKEY_INTER_KEYS_DELAY_MS);

    // 3. Press up the keycode
    minimote_display_key_up_by_linux_keycode(
            display,
            xkb_keycode_to_linux_keycode(keysym_value->keycode));
    msleep(MINIMOTE_HOTKEY_INTER_KEYS_DELAY_MS);

    // 4. Press up the modifiers (backward)
    for (int i = (int) keysym_value->modifiers.size - 1; i >= 0; i--) {
        minimote_display_key_up_by_linux_keycode(
                display,
                xkb_keycode_to_linux_keycode(
                        display->modifiers[keysym_value->modifiers.modifiers[i]]));
        if (i > 0)
            msleep(MINIMOTE_HOTKEY_INTER_KEYS_DELAY_MS);
    }
}

void minimote_display_key_down(minimote_display *display, minimote_key_type key) {
    minimote_display_key_down_by_linux_keycode(display, MINIMOTE_KEY_TO_LINUX_KEY[key]);
}

void minimote_display_key_up(minimote_display *display, minimote_key_type key) {
    minimote_display_key_up_by_linux_keycode(display, MINIMOTE_KEY_TO_LINUX_KEY[key]);
}

void minimote_display_key_click(minimote_display *display, minimote_key_type key) {
    minimote_display_key_click_by_linux_keycode(display, MINIMOTE_KEY_TO_LINUX_KEY[key]);
}

static void minimote_display_key_down_by_linux_keycode(minimote_display *display, int keycode) {
    d("LinuxKeyDown %u", keycode);
    uinput_emit_event_and_syn(display->uinput_fd, EV_KEY, keycode, UINPUT_PRESS);
}

static void minimote_display_key_up_by_linux_keycode(minimote_display *display, int keycode) {
    d("LinuxKeyUp %u", keycode);
    uinput_emit_event_and_syn(display->uinput_fd, EV_KEY, keycode, UINPUT_RELEASE);
}

static void minimote_display_key_click_by_linux_keycode(minimote_display *display, int keycode) {
    minimote_display_key_down_by_linux_keycode(display, keycode);
    minimote_display_key_up_by_linux_keycode(display, keycode);
}