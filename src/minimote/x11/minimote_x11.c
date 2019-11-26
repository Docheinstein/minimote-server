#include "minimote_x11.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include "adt/list/list.h"
#include <minimote/special_keys/keymap/minimote_special_keymap.h>
#include <string.h>
#include <commons/utils/time_utils.h>

#define KEYSYM_POS_MODIFIER_NONE        0
#define KEYSYM_POS_MODIFIER_SHIFTED     1
#define KEYSYM_POS_MODIFIER_NONE_2      2
#define KEYSYM_POS_MODIFIER_SHIFTED_2   3
#define KEYSYM_POS_MODIFIER_ISO_LEVEL3_UNSHIFTED 4
#define KEYSYM_POS_MODIFIER_ISO_LEVEL3_SHIFTED  5

typedef struct keystroke_t {
    KeyCode keycode;
    int keysym_pos;
} keystroke;

static void retrieve_modifiers(minimote_x11 *mx);
static void retrieve_keymap(minimote_x11 *mx);

static void mouse_button_down(minimote_x11 *mx, unsigned int button);
static void mouse_button_up(minimote_x11 *mx, unsigned int button);
static void mouse_button_click(minimote_x11 *mx, unsigned int button);

static void keyboard_key_down(minimote_x11 *mx, unsigned int keycode);
static void keyboard_key_up(minimote_x11 *mx, unsigned int keycode);

static void take_screenshot(minimote_x11 *mx);

static uint32 keys_hash_func(void *data);
static bool keys_key_eq_func(void *hash_key1, void *hash_key2);

static void keymap_visitor(hash_node *hnode) {
    KeySym *keysym = hnode->key;
    keystroke *keystroke = hnode->value;
    printf("Keystroke ===> (K (keysym) = %lu | V (keycode, modifier) = %d, %d\n",
            *keysym, keystroke->keycode, keystroke->keysym_pos);
}

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

    retrieve_modifiers(mx);
    retrieve_keymap(mx);

    hash_foreach(&mx->keymap, keymap_visitor);
}

void minimote_x11_move(minimote_x11 *mx, int dx, int dy) {
    printf("Moving by (dx: %d, y: %d)\n", dx, dy);
    XTestFakeRelativeMotionEvent(mx->display, dx, dy, 0);
    XFlush(mx->display);
}

void minimote_x11_left_down(minimote_x11 *mx) {
    mouse_button_down(mx, Button1);
}

void minimote_x11_left_up(minimote_x11 *mx) {
    mouse_button_up(mx, Button1);
}

void minimote_x11_left_click(minimote_x11 *mx) {
    mouse_button_click(mx, Button1);
}

void minimote_x11_middle_down(minimote_x11 *mx) {
    mouse_button_down(mx, Button2);
}

void minimote_x11_middle_up(minimote_x11 *mx) {
    mouse_button_up(mx, Button2);
}

void minimote_x11_middle_click(minimote_x11 *mx) {
    mouse_button_click(mx, Button2);
}

void minimote_x11_right_down(minimote_x11 *mx) {
//    mouse_button_down(mx, Button3);
}

void minimote_x11_right_up(minimote_x11 *mx) {
//    mouse_button_up(mx, Button3);
    take_screenshot(mx);
}

void minimote_x11_right_click(minimote_x11 *mx) {
    mouse_button_click(mx, Button3);
}

void minimote_x11_scroll_up(minimote_x11 *mx) {
    mouse_button_click(mx, Button4);
}

void minimote_x11_scroll_down(minimote_x11 *mx) {
    mouse_button_click(mx, Button5);
}

static void print_keycode(void *arg) {
    int keycode = *(int *) arg;
    printf("K: %d\n", keycode);
}

void minimote_x11_key_click(minimote_x11 *mx, uint32 unicode_key) {

    // Retrieve the keystroke (keycode + modifier) from the special_keys
    KeySym *k = malloc(sizeof(KeySym));
    *k = unicode_key;
    keystroke *ks = hash_get(&mx->keymap, k);
    free(k);

    if (!ks) {
        fprintf(stderr, "Unknown key for this layout %u\n", unicode_key);
        return;
    }

//    KeyCode modifier_keycode = mx->modifiers[ks->keysym_pos];

    printf("unicode = %d\n", unicode_key);
    printf("keycode = %d\n", ks->keycode);
    printf("modifier_index = %d\n", ks->keysym_pos);
//    printf("modifier = %d\n", modifier_keycode);

    if (ks->keysym_pos == KEYSYM_POS_MODIFIER_SHIFTED ||
        ks->keysym_pos == KEYSYM_POS_MODIFIER_SHIFTED_2) {
        keyboard_key_down(mx, mx->modifiers[ShiftMapIndex]);
    }

    if (ks->keysym_pos == KEYSYM_POS_MODIFIER_ISO_LEVEL3_SHIFTED ||
        ks->keysym_pos == KEYSYM_POS_MODIFIER_ISO_LEVEL3_UNSHIFTED) {
        keyboard_key_down(mx, mx->modifiers[Mod5MapIndex]);
    }

    keyboard_key_down(mx, ks->keycode);
    keyboard_key_up(mx, ks->keycode);

    if (ks->keysym_pos == KEYSYM_POS_MODIFIER_SHIFTED ||
        ks->keysym_pos == KEYSYM_POS_MODIFIER_SHIFTED_2) {
        keyboard_key_up(mx, mx->modifiers[ShiftMapIndex]);
    }

    if (ks->keysym_pos == KEYSYM_POS_MODIFIER_ISO_LEVEL3_SHIFTED ||
        ks->keysym_pos == KEYSYM_POS_MODIFIER_ISO_LEVEL3_UNSHIFTED) {
        keyboard_key_up(mx, mx->modifiers[Mod5MapIndex]);
    }

#if 0
    KeyCode k1 = XKeysymToKeycode(mx->display, XK_A);
    KeyCode k2 = XKeysymToKeycode(mx->display, XK_a);
//    printf("k1: %c", k1);
//    printf("k2: %c", k2);

    XTestFakeKeyEvent(mx->display, 38, True, 0);
    XFlush(mx->display);
    XTestFakeKeyEvent(mx->display, 38, False, 0);
    XFlush(mx->display);
    return;
#endif
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
#if 0
    KeySym keysym_override = unicode_key;
//    KeySym keysym_override = XStringToKeysym("U00C8");
    int k = 8; // CHANGE ME
    KeySym keysym_overrides[] = {
            keysym_override,
            keysym_override,
            keysym_override,
            keysym_override,
            keysym_override,
            keysym_override,
            keysym_override
    };
    printf("Keysym override for %#02x: %s\n", unicode_key, XKeysymToString(keysym_override));
    int res = XChangeKeyboardMapping(mx->display, k, 7, keysym_overrides, 1);
    printf("Result: %d\n", res);

    XFlush(mx->display);
    XSync(mx->display, False);

//    KeyCode keycode_override = XKeysymToKeycode(mx->display, keysym_override);
//    printf("Going to press: %c", keycode_override);

    XTestFakeKeyEvent(mx->display, k, True, 0);
    XFlush(mx->display);
    XTestFakeKeyEvent(mx->display, k, False, 0);

    XFlush(mx->display);
#endif
}

void minimote_x11_special_key_down(minimote_x11 *mx, minimote_special_key_type special_key) {
    KeySym k = SPECIAL_KEY_MAP[special_key];
    keystroke *ks = hash_get(&mx->keymap, &k);

    if (!ks) {
        fprintf(stderr, "Unknown keysym: %lu\n", k);
        return;
    }
    printf("Special key down\n");
    printf("- keysym = %lu\n", k);
    printf("- keycode = %d\n", ks->keycode);
    keyboard_key_down(mx, ks->keycode);
}

void minimote_x11_special_key_up(minimote_x11 *mx, minimote_special_key_type special_key) {
    KeySym k = SPECIAL_KEY_MAP[special_key];
    keystroke *ks = hash_get(&mx->keymap, &k);

    if (!ks) {
        fprintf(stderr, "Unknown keysym: %lu", k);
        return;
    }

    printf("Special key up\n");
    printf("- keysym = %lu\n", k);
    printf("- keycode = %d\n", ks->keycode);
    keyboard_key_up(mx, ks->keycode);
}

void minimote_x11_special_key_click(minimote_x11 *mx, minimote_special_key_type special_key) {
    minimote_x11_special_key_down(mx, special_key);
    minimote_x11_special_key_up(mx, special_key);
}

// _________________________________________________________

void mouse_button_down(minimote_x11 *mx, unsigned int button) {
    XTestFakeButtonEvent(mx->display, button, True, 0);
    XFlush(mx->display);
}

void mouse_button_up(minimote_x11 *mx, unsigned int button) {
    XTestFakeButtonEvent(mx->display, button, False, 0);
    XFlush(mx->display);
}

void mouse_button_click(minimote_x11 *mx, unsigned int button) {
    XTestFakeButtonEvent(mx->display, button, True, 0);
    XTestFakeButtonEvent(mx->display, button, False, 0);
    XFlush(mx->display);
}

void retrieve_modifiers(minimote_x11 *mx) {
    XModifierKeymap *modmap = XGetModifierMapping(mx->display);
    printf("kmap->max_keypermod %d\n", modmap->max_keypermod);
    for (int modifier_index = 0; modifier_index < MODIFIERS_COUNT; modifier_index++) {
        bool modifier_found = false;
        for (int j = 0; j < modmap->max_keypermod; j++) {
            KeyCode kc = modmap->modifiermap[modifier_index * modmap->max_keypermod + j];
            printf("Modifier %d - Key %d => %u\n", modifier_index, j, kc);
            if (kc) {
                printf("- Found modifier %d = %d\n", modifier_index, kc);
                mx->modifiers[modifier_index] = kc;
                modifier_found = true;
                break;
            }
        }

        if (!modifier_found) {
            printf("- Modifier %d not found\n", modifier_index);
            mx->modifiers[modifier_index] = 0;
        }
    }

    XFree(modmap);

    for (int i = 0; i < MODIFIERS_COUNT; i++)
        printf("M %d = %d\n", i, mx->modifiers[i]);
}

void retrieve_keymap(minimote_x11 *mx) {
    printf("Retrieving mapped keys\n");

    int lowest_key;
    int highest_key;

    XDisplayKeycodes(mx->display, &lowest_key, &highest_key);

    printf("lowest_key: %d\n", lowest_key);
    printf("highest_key: %d\n", highest_key);

    int keysyms_per_keycode;
    KeySym *sims = XGetKeyboardMapping(
            mx->display, lowest_key, highest_key - lowest_key, &keysyms_per_keycode);

    printf("keysyms_per_keycode %d\n", keysyms_per_keycode);

    int keycount = (highest_key - lowest_key) * keysyms_per_keycode;

    hash_init(&mx->keymap, keycount * 2, keys_hash_func, keys_key_eq_func);

//    list available_keys;
//    list_init(&available_keys);
    for (int keycode = lowest_key; keycode <= highest_key; keycode++) {
//        bool key_empty = true;
        for (int modifier = 0; modifier <= keysyms_per_keycode; modifier++) {
            int keysym_index = (keycode - lowest_key) * keysyms_per_keycode + modifier;
            KeySym *keysym = malloc(sizeof(KeySym));
            *keysym = sims[keysym_index];
            printf("K[%d(%#02x).%d] = %lu(%#02lx)\n", keycode, keycode, modifier, *keysym, *keysym);
            if (*keysym == NoSymbol)
                continue;

            // Store the association
            keystroke *ks = malloc(sizeof(keystroke));
            ks->keycode = keycode;
            ks->keysym_pos = modifier;

            printf("Pushing keystroke (K (keysym) = %lu | V (keycode, modifier) = %d, %d\n",
                   *keysym, ks->keycode, ks->keysym_pos);

            keystroke *current_ks = hash_get(&mx->keymap, keysym);
            if (!current_ks)
                hash_put(&mx->keymap, keysym, ks);
            else {
                if (ks->keysym_pos < current_ks->keysym_pos) {
                    printf("Replacing keystroke since modifier is easier");
                    hash_put(&mx->keymap, keysym, ks);
                }
            }

            // At least a keysym for this keycode is used: not empty
//                key_empty = false;
//                break;

        }
//        if (!key_empty) {
//            printf("------ [skipping keycode: %d(%#02x)] ---\n", keycode, keycode);
//            continue;
//        }
//
//        printf("- Empty keycode found: %d(%#02x)\n", keycode, keycode);
//        int *keycode_data = malloc(sizeof(int));
//        *keycode_data = keycode;
//        list_append(&available_keys, keycode_data);
    }

    XFree(sims);
}
#if 0
    printf("There are %lu available keys\n", list_size(&available_keys));
    list_foreach(&available_keys, print_keycode);

    list_node *head = list_head(&available_keys);
    list_remove(&available_keys, head);
//    int k = *(int *) head->data;
    int k = 8; // CHANGE ME
    printf("First available key: %d\n", k);

    printf("Now there are less available keys = %lu\n", list_size(&available_keys));
    list_foreach(&available_keys, print_keycode);
#endif

void keyboard_key_down(minimote_x11 *mx, unsigned int keycode) {
    XTestFakeKeyEvent(mx->display, keycode, True, 0);
    XFlush(mx->display);
}

void keyboard_key_up(minimote_x11 *mx, unsigned int keycode) {
    XTestFakeKeyEvent(mx->display, keycode, False, 0);
    XFlush(mx->display);
}

uint32 keys_hash_func(void *data) {
    KeySym *k = data;
    return *k;
}

bool keys_key_eq_func(void *hash_key1, void *hash_key2) {
    KeySym k1 = *(KeySym *) hash_key1;
    KeySym k2 = *(KeySym *) hash_key2;
    return k1 == k2;
}

#define RGB_CHANS 3

#define PPM_MAGIC_NUMBER "P6"
#define PPM_RGB_DEPTH 255

#define rgb_to_gray(r, g, b) \
    (r * RGB_R_TO_GRAY_FACTOR + \
     g * RGB_G_TO_GRAY_FACTOR + \
     b * RGB_B_TO_GRAY_FACTOR)

struct ppm_image_t {
    int width;
    int height;
    int depth;
    unsigned char *pixels;
} typedef ppm_image;

unsigned int ppm_image_bytes(ppm_image *img) {
    return sizeof(unsigned char) * img->width * img->height * RGB_CHANS;
}

void ppm_image_write(ppm_image *ppm_image, const char *image_filename) {
    printf("Writing PPM image %s...\n", image_filename);

    printf("=====\n");

    FILE *image_file = fopen(image_filename, "w");

    // Used string definitions

    const char *P_SIX = PPM_MAGIC_NUMBER;
    const char *WS    = " ";
    const char *NL    = "\n";

    char WIDTH[16];
    char HEIGHT[16];
    char DEPTH[16];

    sprintf(WIDTH, "%d", ppm_image->width);
    sprintf(HEIGHT, "%d", ppm_image->height);
    sprintf(DEPTH, "%d", ppm_image->depth);

    // File writing

    // P6 \n

    fwrite(P_SIX, sizeof(char), strlen(P_SIX), image_file);
    fwrite(NL, sizeof(char), strlen(NL), image_file);

    // <width> <height> <depth> \n

    fwrite(WIDTH, sizeof(char), strlen(WIDTH), image_file);
    fwrite(WS, sizeof(char), strlen(WS), image_file);

    fwrite(HEIGHT, sizeof(char), strlen(HEIGHT), image_file);
    fwrite(WS, sizeof(char), strlen(WS), image_file);

    fwrite(DEPTH, sizeof(char), strlen(DEPTH), image_file);
    fwrite(NL, sizeof(char), strlen(NL), image_file);

    // Pixels

    fwrite(ppm_image->pixels, sizeof(unsigned char),
           RGB_CHANS * ppm_image->width * ppm_image->height, image_file);

    printf("Image %s has been written successfully\n", image_filename);

    printf("=====\n");

    fclose(image_file);
}

void take_screenshot(minimote_x11 *mx) {
    long start = current_ms();

    Window root = DefaultRootWindow(mx->display);

    XWindowAttributes window_attrs;

    XGetWindowAttributes(mx->display, root, &window_attrs);
    int width = window_attrs.width;
    int height = window_attrs.height;

    int W = 400;
    int H = 400;

    XImage *ximg = XGetImage(mx->display, root, 0, 0, W, H, AllPlanes, XYPixmap);

    long post_XGetImage = current_ms() - start;

    printf("post_XGetImage: %lums\n", post_XGetImage);

    unsigned long red_mask = ximg->red_mask;
    unsigned long green_mask = ximg->green_mask;
    unsigned long blue_mask = ximg->blue_mask;

    ppm_image ppm;
    ppm.width = W;
    ppm.height = H;
    ppm.depth = 255;
    const uint image_byte_size = ppm_image_bytes(&ppm);
    ppm.pixels = (unsigned char *) malloc(image_byte_size);

    printf("Image size: %dB\n", image_byte_size);
    printf("Image size: %dMB\n", image_byte_size / 1000000);

    for (int x = 0; x < W; x++) {
        for (int y = 0; y < H ; y++) {
            unsigned long xpixel = XGetPixel(ximg, x, y);

            unsigned char blue = xpixel & blue_mask;
            unsigned char green = (xpixel & green_mask) >> 8;
            unsigned char red = (xpixel & red_mask) >> 16;

            ppm.pixels[(x + W * y) * 3] = red;
            ppm.pixels[(x + W * y) * 3+1] = green;
            ppm.pixels[(x + W * y) * 3+2] = blue;
        }
    }

    long pre_write = current_ms() - start;
    printf("Take screen: %lums\n", pre_write);

    ppm_image_write(&ppm, "/tmp/_.screenshot.ppm");

    long post_write = current_ms() - start;
    printf("Take screen and write: %lums\n", post_write);
}
