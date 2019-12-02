#include "minimote_x11.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include "adt/list/list.h"
#include "minimote/keys/minimote_key_type_x11_map.h"
#include "logging/logging.h"

#define KEYSYM_POS_MODIFIER_NONE        0
#define KEYSYM_POS_MODIFIER_SHIFTED     1
#define KEYSYM_POS_MODIFIER_NONE_2      2
#define KEYSYM_POS_MODIFIER_SHIFTED_2   3
#define KEYSYM_POS_MODIFIER_ISO_LEVEL3_UNSHIFTED 4
#define KEYSYM_POS_MODIFIER_ISO_LEVEL3_SHIFTED  5

minimote_x11 X11;

typedef struct keystroke_t {
    KeyCode keycode;
    int keysym_pos;
} keystroke;

static void minimote_x11_retrieve_modifiers(minimote_x11 *mx);
static void minimote_x11_retrieve_keymap(minimote_x11 *mx);

static void minimote_x11_mouse_button_down(minimote_x11 *mx, unsigned int button);
static void minimote_x11_mouse_button_up(minimote_x11 *mx, unsigned int button);
static void minimote_x11_mouse_button_click(minimote_x11 *mx, unsigned int button);

static void minimote_x11_keyboard_key_down(minimote_x11 *mx, unsigned int keycode);
static void minimote_x11_keyboard_key_up(minimote_x11 *mx, unsigned int keycode);

//static void take_screenshot(minimote_x11 *mx);

// Hash functions

static uint32 keymap_hash_func(void *data);
static bool keymap_key_eq_func(void *hash_key1, void *hash_key2);
static void keysym_free_func(void *arg);
static void keystroke_free_func(void *arg);

static void keymap_iterator_dump(hash_node *hnode, void *arg);

void minimote_x11_init(minimote_x11 *mx) {
    mx->display = XOpenDisplay(NULL);

    if (!mx->display) {
        e("Unable to open display");
        exit(-1);
    }

    int event_basep, error_basep, majorp, minorp;
    if (XTestQueryExtension(mx->display, &event_basep, &error_basep, &majorp, &minorp) != True) {
        e("Environment does not support x11 TEST extension, please install X11tst");
        exit(-1);
    }

    minimote_x11_retrieve_modifiers(mx);
    minimote_x11_retrieve_keymap(mx);

    hash_foreach(&mx->keymap, keymap_iterator_dump, NULL);
}

void minimote_x11_move(minimote_x11 *mx, int dx, int dy) {
    d("Moving by (dx: %d, y: %d)", dx, dy);
    XTestFakeRelativeMotionEvent(mx->display, dx, dy, 0);
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

void minimote_x11_write(minimote_x11 *mx, uint32 unicode_key) {
    // Retrieve the keystroke (keycode + modifier) from the keys
    KeySym k = unicode_key;
    keystroke *ks = hash_get(&mx->keymap, &k);

    if (!ks) {
        w("Unknown key for this layout %u", unicode_key);
        return;
    }
    d("Writing");
    d("- unicode = %d", unicode_key);
    d("- keycode = %d", ks->keycode);
    d("- modifier_index = %d", ks->keysym_pos);

    if (ks->keysym_pos == KEYSYM_POS_MODIFIER_SHIFTED ||
        ks->keysym_pos == KEYSYM_POS_MODIFIER_SHIFTED_2) {
        minimote_x11_keyboard_key_down(mx, mx->modifiers[ShiftMapIndex]);
    }

    if (ks->keysym_pos == KEYSYM_POS_MODIFIER_ISO_LEVEL3_SHIFTED ||
        ks->keysym_pos == KEYSYM_POS_MODIFIER_ISO_LEVEL3_UNSHIFTED) {
        minimote_x11_keyboard_key_down(mx, mx->modifiers[Mod5MapIndex]);
    }

    minimote_x11_keyboard_key_down(mx, ks->keycode);
    minimote_x11_keyboard_key_up(mx, ks->keycode);

    if (ks->keysym_pos == KEYSYM_POS_MODIFIER_SHIFTED ||
        ks->keysym_pos == KEYSYM_POS_MODIFIER_SHIFTED_2) {
        minimote_x11_keyboard_key_up(mx, mx->modifiers[ShiftMapIndex]);
    }

    if (ks->keysym_pos == KEYSYM_POS_MODIFIER_ISO_LEVEL3_SHIFTED ||
        ks->keysym_pos == KEYSYM_POS_MODIFIER_ISO_LEVEL3_UNSHIFTED) {
        minimote_x11_keyboard_key_up(mx, mx->modifiers[Mod5MapIndex]);
    }
}

void minimote_x11_key_down(minimote_x11 *mx, minimote_key_type special_key) {
    KeySym k = KEY_TYPE_X11_MAP[special_key];
    keystroke *ks = hash_get(&mx->keymap, &k);

    if (!ks) {
        w("Unknown keysym: %lu", k);
        return;
    }

    d("Key down");
    d("- keysym = %lu", k);
    d("- keycode = %d", ks->keycode);
    minimote_x11_keyboard_key_down(mx, ks->keycode);
}

void minimote_x11_key_up(minimote_x11 *mx, minimote_key_type special_key) {
    KeySym k = KEY_TYPE_X11_MAP[special_key];
    keystroke *ks = hash_get(&mx->keymap, &k);

    if (!ks) {
        w("Unknown keysym: %lu", k);
        return;
    }

    d("Key up");
    d("- keysym = %lu", k);
    d("- keycode = %d", ks->keycode);
    minimote_x11_keyboard_key_up(mx, ks->keycode);
}

void minimote_x11_key_click(minimote_x11 *mx, minimote_key_type special_key) {
    minimote_x11_key_down(mx, special_key);
    minimote_x11_key_up(mx, special_key);
}

// _________________________________________________________

void minimote_x11_mouse_button_down(minimote_x11 *mx, unsigned int button) {
    XTestFakeButtonEvent(mx->display, button, True, 0);
    XFlush(mx->display);
}

void minimote_x11_mouse_button_up(minimote_x11 *mx, unsigned int button) {
    XTestFakeButtonEvent(mx->display, button, False, 0);
    XFlush(mx->display);
}

void minimote_x11_mouse_button_click(minimote_x11 *mx, unsigned int button) {
    XTestFakeButtonEvent(mx->display, button, True, 0);
    XTestFakeButtonEvent(mx->display, button, False, 0);
    XFlush(mx->display);
}

void minimote_x11_retrieve_modifiers(minimote_x11 *mx) {
    XModifierKeymap *modmap = XGetModifierMapping(mx->display);
    d("kmap->max_keypermod %d", modmap->max_keypermod);

    for (int modifier_index = 0; modifier_index < MODIFIERS_COUNT; modifier_index++) {
        bool modifier_found = false;
        for (int j = 0; j < modmap->max_keypermod; j++) {
            KeyCode kc = modmap->modifiermap[modifier_index * modmap->max_keypermod + j];
            d("Modifier %d - Key %d => %u", modifier_index, j, kc);
            if (kc) {
                d("- Found modifier %d = %d", modifier_index, kc);
                mx->modifiers[modifier_index] = kc;
                modifier_found = true;
                break;
            }
        }

        if (!modifier_found) {
            d("- Modifier %d not found", modifier_index);
            mx->modifiers[modifier_index] = 0;
        }
    }

    XFree(modmap);

    for (int i = 0; i < MODIFIERS_COUNT; i++)
        d("M %d = %d", i, mx->modifiers[i]);
}

void minimote_x11_retrieve_keymap(minimote_x11 *mx) {
    d("Retrieving mapped keys");

    int lowest_key;
    int highest_key;

    XDisplayKeycodes(mx->display, &lowest_key, &highest_key);

    d("lowest_key: %d", lowest_key);
    d("highest_key: %d", highest_key);

    int keysyms_per_keycode;
    KeySym *sims = XGetKeyboardMapping(
            mx->display, lowest_key, highest_key - lowest_key, &keysyms_per_keycode);

    d("keysyms_per_keycode %d", keysyms_per_keycode);

    int keycount = (highest_key - lowest_key) * keysyms_per_keycode;

    hash_init_full(
            &mx->keymap, keycount * 2,
            keymap_hash_func, keymap_key_eq_func,
            keysym_free_func, keystroke_free_func
    );

    for (int keycode = lowest_key; keycode <= highest_key; keycode++) {
        for (int modifier = 0; modifier <= keysyms_per_keycode; modifier++) {
            int keysym_index = (keycode - lowest_key) * keysyms_per_keycode + modifier;

            if (sims[keysym_index] == NoSymbol)
                continue;

            keystroke *current_ks = hash_get(&mx->keymap, &sims[keysym_index]);
            if (current_ks && modifier >= current_ks->keysym_pos) {
                d("Nothing to push, keystroke already exist and it's easier than this one");
                continue;
            }
            if (!current_ks)
                d("- pushing keystroke for first time");
            else if (modifier < current_ks->keysym_pos)
                d("- replacing keystroke since modifier is easier");

            // We will insert the keystroke

            KeySym *keysym = malloc(sizeof(KeySym));
            *keysym = sims[keysym_index];

            keystroke *ks = malloc(sizeof(keystroke));
            ks->keycode = keycode;
            ks->keysym_pos = modifier;

            v("[K (keysym) = %lu -> V (keycode, modifier) = %d, %d]",
              *keysym, ks->keycode, ks->keysym_pos);

            hash_put(&mx->keymap, keysym, ks);
        }
    }

    XFree(sims);
}

void minimote_x11_keyboard_key_down(minimote_x11 *mx, unsigned int keycode) {
    XTestFakeKeyEvent(mx->display, keycode, True, 0);
    XFlush(mx->display);
}

void minimote_x11_keyboard_key_up(minimote_x11 *mx, unsigned int keycode) {
    XTestFakeKeyEvent(mx->display, keycode, False, 0);
    XFlush(mx->display);
}

uint32 keymap_hash_func(void *data) {
    KeySym *k = data;
    return *k;
}

bool keymap_key_eq_func(void *hash_key1, void *hash_key2) {
    KeySym k1 = *(KeySym *) hash_key1;
    KeySym k2 = *(KeySym *) hash_key2;
    return k1 == k2;
}

void keymap_iterator_dump(hash_node *hnode, void *arg) {
    KeySym *keysym = hnode->key;
    keystroke *keystroke = hnode->value;
    d("Keystroke ===> (K (keysym) = %lu | V (keycode, modifier) = %d, %d",
      *keysym, keystroke->keycode, keystroke->keysym_pos);
}

void keysym_free_func(void *arg) {
    free(arg);
}

void keystroke_free_func(void *arg) {
    keystroke *ks = (keystroke *) arg;
    d("Freeing keystroke: %d, %d", ks->keycode, ks->keysym_pos);
    free(arg);
}
//
//#define RGB_CHANS 3
//
//#define PPM_MAGIC_NUMBER "P6"
//#define PPM_RGB_DEPTH 255
//
//#define rgb_to_gray(r, g, b) \
//    (r * RGB_R_TO_GRAY_FACTOR + \
//     g * RGB_G_TO_GRAY_FACTOR + \
//     b * RGB_B_TO_GRAY_FACTOR)
//
//struct ppm_image_t {
//    int width;
//    int height;
//    int depth;
//    unsigned char *pixels;
//} typedef ppm_image;
//
//unsigned int ppm_image_bytes(ppm_image *img) {
//    return sizeof(unsigned char) * img->width * img->height * RGB_CHANS;
//}
//
//void ppm_image_write(ppm_image *ppm_image, const char *image_filename) {
//    printf("Writing PPM image %s...\n", image_filename);
//
//    printf("=====\n");
//
//    FILE *image_file = fopen(image_filename, "w");
//
//    // Used string definitions
//
//    const char *P_SIX = PPM_MAGIC_NUMBER;
//    const char *WS    = " ";
//    const char *NL    = "\n";
//
//    char WIDTH[16];
//    char HEIGHT[16];
//    char DEPTH[16];
//
//    sprintf(WIDTH, "%d", ppm_image->width);
//    sprintf(HEIGHT, "%d", ppm_image->height);
//    sprintf(DEPTH, "%d", ppm_image->depth);
//
//    // File writing
//
//    // P6 \n
//
//    fwrite(P_SIX, sizeof(char), strlen(P_SIX), image_file);
//    fwrite(NL, sizeof(char), strlen(NL), image_file);
//
//    // <width> <height> <depth> \n
//
//    fwrite(WIDTH, sizeof(char), strlen(WIDTH), image_file);
//    fwrite(WS, sizeof(char), strlen(WS), image_file);
//
//    fwrite(HEIGHT, sizeof(char), strlen(HEIGHT), image_file);
//    fwrite(WS, sizeof(char), strlen(WS), image_file);
//
//    fwrite(DEPTH, sizeof(char), strlen(DEPTH), image_file);
//    fwrite(NL, sizeof(char), strlen(NL), image_file);
//
//    // Pixels
//
//    fwrite(ppm_image->pixels, sizeof(unsigned char),
//           RGB_CHANS * ppm_image->width * ppm_image->height, image_file);
//
//    printf("Image %s has been written successfully\n", image_filename);
//
//    printf("=====\n");
//
//    fclose(image_file);
//}
//
//void take_screenshot(minimote_x11 *mx) {
//    long start = current_ms();
//
//    Window root = DefaultRootWindow(mx->display);
//
//    XWindowAttributes window_attrs;
//
//    XGetWindowAttributes(mx->display, root, &window_attrs);
//    int width = window_attrs.width;
//    int height = window_attrs.height;
//
//    int W = 400;
//    int H = 400;
//
//    XImage *ximg = XGetImage(mx->display, root, 0, 0, W, H, AllPlanes, XYPixmap);
//
//    long post_XGetImage = current_ms() - start;
//
//    printf("post_XGetImage: %lums\n", post_XGetImage);
//
//    unsigned long red_mask = ximg->red_mask;
//    unsigned long green_mask = ximg->green_mask;
//    unsigned long blue_mask = ximg->blue_mask;
//
//    ppm_image ppm;
//    ppm.width = W;
//    ppm.height = H;
//    ppm.depth = 255;
//    const uint image_byte_size = ppm_image_bytes(&ppm);
//    ppm.pixels = (unsigned char *) malloc(image_byte_size);
//
//    printf("Image size: %dB\n", image_byte_size);
//    printf("Image size: %dMB\n", image_byte_size / 1000000);
//
//    for (int x = 0; x < W; x++) {
//        for (int y = 0; y < H ; y++) {
//            unsigned long xpixel = XGetPixel(ximg, x, y);
//
//            unsigned char blue = xpixel & blue_mask;
//            unsigned char green = (xpixel & green_mask) >> 8;
//            unsigned char red = (xpixel & red_mask) >> 16;
//
//            ppm.pixels[(x + W * y) * 3] = red;
//            ppm.pixels[(x + W * y) * 3+1] = green;
//            ppm.pixels[(x + W * y) * 3+2] = blue;
//        }
//    }
//
//    long pre_write = current_ms() - start;
//    printf("Take screen: %lums\n", pre_write);
//
//    ppm_image_write(&ppm, "/tmp/_.screenshot.ppm");
//
//    long post_write = current_ms() - start;
//    printf("Take screen and write: %lums\n", post_write);
//}
