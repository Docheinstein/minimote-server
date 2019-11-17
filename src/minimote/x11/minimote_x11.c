#include "minimote_x11.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include "adt/list/list.h"
#include <X11/keysymdef.h>
void minimote_x11_init(minimote_x11 *mx) {
    mx->display = XOpenDisplay(NULL);

    if (!mx->display) {
        fprintf(stderr, "Unable to open display\n");
        exit(-1);
    }

    int event_basep, error_basep, majorp, minorp;
    if (XTestQueryExtension(mx->display, &event_basep, &error_basep, &majorp, &minorp) != True) {
        fprintf(stderr, "Environment does not support x11 TEST extension, please install X11tst\n");
        exit(-1);
    }
}

void minimote_x11_move(minimote_x11 *mx, int dx, int dy) {
    printf("Moving by (dx: %d, y: %d)\n", dx, dy);
    XTestFakeRelativeMotionEvent(mx->display, dx, dy, 0);
    XFlush(mx->display);
}

static void minimote_x11_mouse_button_down(minimote_x11 *mx, unsigned int button) {
    XTestFakeButtonEvent(mx->display, button, True, 0);
    XFlush(mx->display);
}

static void minimote_x11_mouse_button_up(minimote_x11 *mx, unsigned int button) {
    XTestFakeButtonEvent(mx->display, button, False, 0);
    XFlush(mx->display);
}

static void minimote_x11_mouse_button_click(minimote_x11 *mx, unsigned int button) {
    XTestFakeButtonEvent(mx->display, button, True, 0);
    XTestFakeButtonEvent(mx->display, button, False, 0);
    XFlush(mx->display);
}

void minimote_x11_left_down(minimote_x11 *mx) {
    minimote_x11_mouse_button_down(mx, Button1);
}

void minimote_x11_left_up(minimote_x11 *mx) {
    minimote_x11_mouse_button_up(mx, Button1);
}

void minimote_x11_left_click(minimote_x11 *mx) {
    minimote_x11_mouse_button_click(mx, Button1);
}

void minimote_x11_middle_down(minimote_x11 *mx) {
    minimote_x11_mouse_button_down(mx, Button2);
}

void minimote_x11_middle_up(minimote_x11 *mx) {
    minimote_x11_mouse_button_up(mx, Button2);
}

void minimote_x11_middle_click(minimote_x11 *mx) {
    minimote_x11_mouse_button_click(mx, Button2);
}

void minimote_x11_right_down(minimote_x11 *mx) {
    minimote_x11_mouse_button_down(mx, Button3);
}

void minimote_x11_right_up(minimote_x11 *mx) {
    minimote_x11_mouse_button_up(mx, Button3);
}

void minimote_x11_right_click(minimote_x11 *mx) {
    minimote_x11_mouse_button_click(mx, Button3);
}

void minimote_x11_scroll_up(minimote_x11 *mx) {
    minimote_x11_mouse_button_click(mx, Button4);
}

void minimote_x11_scroll_down(minimote_x11 *mx) {
    minimote_x11_mouse_button_click(mx, Button5);
}

static void print_keycode(void *arg) {
    int keycode = *(int *) arg;
    printf("K: %d\n", keycode);
}

void minimote_x11_key_click(minimote_x11 *mx, uint32 unicode_key) {
#if 0
    int lowest_key;
    int highest_key;

    XDisplayKeycodes(mx->display, &lowest_key, &highest_key);

    printf("Min keycode: %d\n", lowest_key);
    printf("Max keycode: %d\n", highest_key);

    int keysyms_per_keycode;
    KeySym * sims = XGetKeyboardMapping(
            mx->display, lowest_key, highest_key - lowest_key, &keysyms_per_keycode);

    printf("keysyms_per_keycode %d\n", keysyms_per_keycode);

    list available_keys;
    list_init(&available_keys);

    for (int keycode = lowest_key; keycode < highest_key; keycode++) {
        bool key_empty = true;
        for (int keysym = 0; keysym < keysyms_per_keycode; keysym++) {
            int keysym_index = (keycode - lowest_key) * keysyms_per_keycode + keysym;
            ulong keysym_value = sims[keysym_index];
            printf("K[%d(%#02x).%d] = %lu\n", keycode, keycode, keysym, keysym_value);
            if (keysym_value != NoSymbol) {
                // At least a keysym for this keycode is used: not empty
                key_empty = false;
                break;
            }
        }
        if (!key_empty)
            continue;

        printf("- Empty keycode found: %d(%#02x)\n", keycode, keycode);
        int *keycode_data = malloc(sizeof(int));
        *keycode_data = keycode;
        list_append(&available_keys, keycode_data);
    }

    printf("There are %lu available keys\n", list_size(&available_keys));
    list_foreach(&available_keys, print_keycode);

    list_node *head = list_head(&available_keys);
    list_remove(&available_keys, head);
//    int k = *(int *) head->data;
    int k = 8; // CHANGE ME
    printf("First available key: %d\n", k);

    printf("Now there are less available keys = %lu\n", list_size(&available_keys));
    list_foreach(&available_keys, print_keycode);

    XFree(sims);
#endif
    KeySym keysym_override = 0x69;
//    KeySym keysym_override = XStringToKeysym("U00C8");
    int k = 8; // CHANGE ME
    KeySym keysym_overrides[] = {
            keysym_override
    };
    printf("Keysym override for %#02x: %s\n", unicode_key, XKeysymToString(keysym_override));
    XChangeKeyboardMapping(mx->display, k, 1, keysym_overrides, 1);

    XFlush(mx->display);

//    KeyCode keycode_override = XKeysymToKeycode(mx->display, keysym_override);
//    printf("Going to press: %c", keycode_override);

    XTestFakeKeyEvent(mx->display, k, True, 0);
    XTestFakeKeyEvent(mx->display, k, False, 0);

    XFlush(mx->display);
}
