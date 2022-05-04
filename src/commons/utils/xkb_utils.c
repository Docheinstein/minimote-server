#include "xkb_utils.h"
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "commons/containers/hash/hash.h"
#include "io_utils.h"
#include "log/log.h"
#include "str_utils.h"

#ifdef X11
#include <xcb/xcb.h>
#include <xkbcommon/xkbcommon-x11.h>
#endif

#ifdef WAYLAND
#include "wayland-client.h"
#endif

typedef struct xkb_keymap_get_mod_key_parse_callback_arg {
    struct {
        // (const char *) -> int
        // e.g. <LCTL> -> 37
        hash symbols;

        // (const char *) -> (const char *)
        // e.g. Shift -> <LCTL>
        hash modifiers;
    } result;

    bool parsing_keycodes;

} xkb_keymap_get_mod_key_parse_callback_arg;

static char * xkb_keymap_get_modifiers_mapping_parse_callback(const char *line, void *arg_) {
    d(">> %s", line);

    xkb_keymap_get_mod_key_parse_callback_arg *arg = (xkb_keymap_get_mod_key_parse_callback_arg*) arg_;

    if (!arg->parsing_keycodes && strstr(line, "xkb_keycodes")) {
        arg->parsing_keycodes = true;
        return NULL;
    }

    if (arg->parsing_keycodes && strstr(line, "};")) {
        arg->parsing_keycodes = false;
        return NULL;
    }

    // Look for symbol lines, such as:
    // <AE01> = 10;
    if (arg->parsing_keycodes) {
        char *tokens[4];
        char *line_copy = strdup(line);

        int n_tokens = strsplit(line_copy, " ", tokens, 4);

        if (n_tokens == 3) {
            char *symbol_raw = strdup(tokens[0]);
            char *keycode_raw = strdup(tokens[2]);

            char *symbol = strtrim(symbol_raw);
            char *keycode = strtrim_chars(strtrim(keycode_raw), ";");

            bool inserted = false;

            if (strstarts(symbol, '<') && strends(symbol, '>')) {
                int *keycode_value = malloc(sizeof(int));
                if (!strtoint(keycode, keycode_value)) {
                    char *err = strmake("failed to parse int '%s'", keycode);
                    free(symbol_raw);
                    free(keycode_raw);
                    free(line_copy);
                    return err;
                }

                hash_put(&arg->result.symbols, symbol, keycode_value);
                inserted = true;
            }

            if (!inserted)
                free(symbol_raw);
            free(keycode_raw);
        }

        free(line_copy);
    }

    // Look for modifier lines, such as:
    // modifier_map Control { <LCTL> };
    if (strstr(line, "modifier_map")) {
        char *tokens[16];
        char *line_copy = strdup(line);

        int n_tokens = strsplit(line_copy, " ", tokens, 32);
        if (n_tokens >= 4) {
            char *modifier_raw = strdup(tokens[1]);
            char *symbol_raw = strdup(tokens[3]);
            char *modifier = strtrim(modifier_raw);
            char *symbol = strtrim_chars(strtrim(symbol_raw), ",");
            if (!hash_contains(&arg->result.modifiers, &modifier)) {
                hash_put(&arg->result.modifiers, modifier, symbol);
            } else {
                free(modifier_raw);
                free(symbol_raw);
            }
        }
        free(line_copy);
    }

    return NULL;
}

// http://www.cse.yorku.ca/~oz/hash.html
static uint32 str_hash_func(void *arg) {
    const char *str = arg;
    unsigned long hash = 5381;
    int c;

    while ((c = (unsigned char) *str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}

static bool int_eq_func(void *arg1, void *arg2) {
    return *(int *) arg1 == *(int *) arg2;
}

static bool str_eq_func(void *arg1, void *arg2) {
    return streq((const char *) arg1, (const char *) arg2);
}

xkb_modifiers_mapping xkb_keymap_get_modifiers_mapping(struct xkb_keymap *keymap) {
    /*
     * Unfortunately xkbcommon does not provide a method for retrieve the
     * mapping between the modifiers and the keycode needed to generate them,
     * therefore we have to parse then manually from the keymap file.
     * Specifically, we are looking for lines such as:
     *     modifier_map Control { <LCTL> };
     * In order to resolve the mapping between the symbol (e.g. <LCTL>) and
     * its keycode we have to parse also the symbols sections, which contains
     * lines in the form:
     *     <AE01> = 10;
     * Then we can compute the mapping between the modifiers and their keycodes.
     */

    xkb_modifiers_mapping mapping = NULL;

    char *keymap_str = xkb_keymap_get_as_string(keymap, XKB_KEYMAP_USE_ORIGINAL_FORMAT);

    parse_options parse_opts = parse_options_default();

    xkb_keymap_get_mod_key_parse_callback_arg arg;
    arg.parsing_keycodes = false;
    hash_init_full(&arg.result.symbols, 256, str_hash_func, int_eq_func, free, free);
    hash_init_full(&arg.result.modifiers, 8, str_hash_func, str_eq_func, free, free);

    // Parse the keymap file for both symbols and modifiers

    char *err = string_parse(keymap_str, parse_opts, xkb_keymap_get_modifiers_mapping_parse_callback, &arg);
    if (err) {
        e("Failed to parse keymap: %s", err);
        goto END;
    }

    // Resolve the mapping between the modifiers and their keycodes

    mapping = malloc(XKB_MODIFIERS_NUM * sizeof(xkb_keycode_t));
    for (xkb_mod_index_t i = 0; i < XKB_MODIFIERS_NUM; i++)
        mapping[i] = XKB_KEY_NoSymbol;

    void *shift_symbol = hash_get(&arg.result.modifiers, XKB_MOD_NAME_SHIFT);
    if (shift_symbol) {
        void *shift_keycode = hash_get(&arg.result.symbols, shift_symbol);
        if (shift_keycode)
            mapping[XKB_MOD_INDEX_SHIFT] = *((xkb_keycode_t *) shift_keycode);
    }
    
    void *lock_symbol = hash_get(&arg.result.modifiers, XKB_MOD_NAME_LOCK);
    if (lock_symbol) {
        void *lock_keycode = hash_get(&arg.result.symbols, lock_symbol);
        if (lock_keycode)
            mapping[XKB_MOD_INDEX_LOCK] = *((xkb_keycode_t *) lock_keycode);
    }
        
    void *control_symbol = hash_get(&arg.result.modifiers, XKB_MOD_NAME_CONTROL);
    if (control_symbol) {
        void *control_keycode = hash_get(&arg.result.symbols, control_symbol);
        if (control_keycode)
            mapping[XKB_MOD_INDEX_CONTROL] = *((xkb_keycode_t *) control_keycode);
    }
    
    void *mod1_symbol = hash_get(&arg.result.modifiers, XKB_MOD_NAME_MOD1);
    if (mod1_symbol) {
        void *mod1_keycode = hash_get(&arg.result.symbols, mod1_symbol);
        if (mod1_keycode)
            mapping[XKB_MOD_INDEX_MOD1] = *((xkb_keycode_t *) mod1_keycode);
    }
        
    void *mod2_symbol = hash_get(&arg.result.modifiers, XKB_MOD_NAME_MOD2);
    if (mod2_symbol) {
        void *mod2_keycode = hash_get(&arg.result.symbols, mod2_symbol);
        if (mod2_keycode)
            mapping[XKB_MOD_INDEX_MOD2] = *((xkb_keycode_t *) mod2_keycode);
    }
            
    void *mod3_symbol = hash_get(&arg.result.modifiers, XKB_MOD_NAME_MOD3);
    if (mod3_symbol) {
        void *mod3_keycode = hash_get(&arg.result.symbols, mod3_symbol);
        if (mod3_keycode)
            mapping[XKB_MOD_INDEX_MOD3] = *((xkb_keycode_t *) mod3_keycode);
    }
            
    void *mod4_symbol = hash_get(&arg.result.modifiers, XKB_MOD_NAME_MOD4);
    if (mod4_symbol) {
        void *mod4_keycode = hash_get(&arg.result.symbols, mod4_symbol);
        if (mod4_keycode)
            mapping[XKB_MOD_INDEX_MOD4] = *((xkb_keycode_t *) mod4_keycode);
    }
            
    void *mod5_symbol = hash_get(&arg.result.modifiers, XKB_MOD_NAME_MOD5);
    if (mod5_symbol) {
        void *mod5_keycode = hash_get(&arg.result.symbols, mod5_symbol);
        if (mod5_keycode)
            mapping[XKB_MOD_INDEX_MOD5] = *((xkb_keycode_t *) mod5_keycode);
    }
    
END:
    free(keymap_str);
    hash_destroy(&arg.result.symbols);
    hash_destroy(&arg.result.modifiers);

    return mapping;
}

const char *xkb_mod_get_name(xkb_mod_index_t mod) {
    switch (mod) {
        case XKB_MOD_INDEX_SHIFT:
            return XKB_MOD_NAME_SHIFT;
        case XKB_MOD_INDEX_LOCK:
            return XKB_MOD_NAME_LOCK;
        case XKB_MOD_INDEX_CONTROL:
            return XKB_MOD_NAME_CONTROL;
        case XKB_MOD_INDEX_MOD1:
            return XKB_MOD_NAME_MOD1;
        case XKB_MOD_INDEX_MOD2:
            return XKB_MOD_NAME_MOD2;
        case XKB_MOD_INDEX_MOD3:
            return XKB_MOD_NAME_MOD3;
        case XKB_MOD_INDEX_MOD4:
            return XKB_MOD_NAME_MOD4;
        case XKB_MOD_INDEX_MOD5:
            return XKB_MOD_NAME_MOD5;
        default:
            return "Unknown";
    }
}

static uint32 xkb_keysym_hash_func(void *arg) {
    xkb_keysym_t *keysym = arg;
    return *keysym;
}

static bool xkb_keysym_eq_func(void *arg1, void *arg2) {
    return *((xkb_keysym_t *) arg1) == *((xkb_keysym_t *) arg2);
}

static void xkb_keysyms_mapping_value_free(void *arg) {
    xkb_keysyms_mapping_value *value = arg;
    free(value->modifiers.modifiers);
    free(value);
}

xkb_keysyms_mapping xkb_keymap_get_keysyms_mapping(struct xkb_keymap *keymap) {
    /*
     * Here we want to provide a mapping between the keysyms and the combination
     * of modifiers and keycode needed to generate them.
     * We can use xkbcommon APIs to figure it out.
     *
     * Recap:
     * Each keycode has one or more shift level.
     * Each shift level can be produced with one or more modifiers mask
     * (which is a combination of one or more modifiers).
     * Each combination of keycode + shift level produces one or more keysym
     * (typically one).
     *
     * Only the current/default layout is taken into account (the one with index 0).
     *
     * e.g.
     *
     * For:
     *
     * Key 26
     * - ShiftLevel 0 : Mod 0 -> 101 (e)
     * - ShiftLevel 1 : Mod 1 -> 69 (E)
     * - ShiftLevel 1 : Mod 2 -> 69 (E)
     * - ShiftLevel 2 : Mod 128 -> 8364 (EuroSign)
     * - ShiftLevel 2 : Mod 130 -> 8364 (EuroSign)
     * - ShiftLevel 3 : Mod 129 -> 162 (cent)
     * - ShiftLevel 3 : Mod 131 -> 162 (cent)
     *
     * We will have the mapping:
     * (e)          -> Modifiers=[],                        Keycode=26
     * (E)          -> Modifiers=[Shift (1)],               Keycode=26
     * (E           -> Modifiers=[Lock (2)],                Keycode=26
     * (EuroSign)   -> Modifiers=[Mod5 (128)],              Keycode=26
     * (EuroSign)   -> Modifiers=[Mod5+Lock (130)],         Keycode=26
     * (cent)       -> Modifiers=[Mod5+Shift (129)],        Keycode=26
     * (cent)       -> Modifiers=[Mod5+Shift+Lock (131)],   Keycode=26
     */

    xkb_keycode_t min_keycode = xkb_keymap_min_keycode(keymap);
    xkb_keycode_t max_keycode = xkb_keymap_max_keycode(keymap);

    xkb_keysyms_mapping mapping;

    hash_init_full(&mapping, ((int) max_keycode - (int) min_keycode) * 8 /* heuristic */,
                   xkb_keysym_hash_func, xkb_keysym_eq_func,
                   free, xkb_keysyms_mapping_value_free);


    for (xkb_keycode_t keycode = min_keycode; keycode < max_keycode; keycode++) {
        d("Key %u", keycode);
        // Retrieve the shift levels for this key
        xkb_level_index_t num_levels = xkb_keymap_num_levels_for_key(keymap, keycode, XKB_DEFAULT_LAYOUT_INDEX);
        for (xkb_level_index_t level = 0; level < num_levels; level++) {
            const xkb_keysym_t *syms;
            // Retrieve the symbols for this shift level
            int num_syms = xkb_keymap_key_get_syms_by_level(keymap, keycode, XKB_DEFAULT_LAYOUT_INDEX, level, &syms);

            int MAX_NUM_MODS = 8;
            xkb_mod_mask_t mods[MAX_NUM_MODS];

            // Retrieve the mods for this shift level
            size_t num_mods = xkb_keymap_key_get_mods_for_level(keymap, keycode, XKB_DEFAULT_LAYOUT_INDEX, level, mods, MAX_NUM_MODS);

            for (int mod_i = 0; mod_i < num_mods; mod_i++) {
                xkb_mod_mask_t mod_mask = mods[mod_i];
                for (int sym_i = 0; sym_i < num_syms; sym_i++) {
                    xkb_keysym_t sym = syms[sym_i];
                    char sym_name[32];
                    xkb_keysym_get_name(sym, sym_name, 32);

                    d("- ShiftLevel %u : Mod %u -> %u (%s)", level, mod_mask, sym, sym_name);

                    // Extract the mods indices from the mask
                    // e.g. 130 -> [2^1 + 2^7] = [1, 7]
                    xkb_mod_index_t modifiers[XKB_MODIFIERS_NUM];
                    size_t n_modifiers = 0;

                    for (int i = 0; i < XKB_MODIFIERS_NUM; i++) {
                        if (mod_mask & (1 << i)) {
                            modifiers[n_modifiers++] = i;
                        }
                    }

                    // Insert the mapping for the keysym in two cases:
                    // 1. The keysym is not present in the mapping
                    // 2. The keysym is present in the mapping but the new
                    //    mapping uses a fewer number of modifiers
                    //    (since is "easier" to type)
                    xkb_keysyms_mapping_value *prev_value = hash_get(&mapping, &sym);

                    if (!prev_value) {
                        // Insert the new mapping
                        xkb_keysym_t *keysym = malloc(sizeof(xkb_keysym_t));
                        *keysym = sym;

                        xkb_keysyms_mapping_value *value = malloc(sizeof(xkb_keysyms_mapping_value));
                        value->keycode = keycode;
                        value->modifiers.size = n_modifiers;
                        value->modifiers.modifiers = malloc(n_modifiers * sizeof(xkb_mod_index_t));
                        for (int i = 0; i < n_modifiers; i++)
                            value->modifiers.modifiers[i] = modifiers[i];

                        hash_put(&mapping, keysym, value);
                    } else if (n_modifiers < prev_value->modifiers.size) {
                        // Just update the modifiers array
                        prev_value->modifiers.size = n_modifiers;
                        for (int i = 0; i < n_modifiers; i++)
                            prev_value->modifiers.modifiers[i] = modifiers[i];
                    }
                }
            }
        }
    }

    return mapping;
}

#ifdef X11
struct xkb_keymap * xkb_get_keymap_x11(struct xkb_context *context) {
    /* For X11 we can use the xkbcommon API xkb_x11_get_core_keyboard_device_id
     * to fetch the keymap. */
    d("Connecting to X server...");

    xcb_connection_t *x11_connection = xcb_connect(NULL, NULL);
    if (!x11_connection || xcb_connection_has_error(x11_connection)) {
        e("Failed to connect to X server: error code %d",
          x11_connection? xcb_connection_has_error(x11_connection) : -1);
        return NULL;
    }

    d("Connected to X server");

    if (!xkb_x11_setup_xkb_extension(x11_connection,
                                     XKB_X11_MIN_MAJOR_XKB_VERSION,
                                     XKB_X11_MIN_MINOR_XKB_VERSION,
                                     XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS,
                                     NULL, NULL, NULL, NULL)) {
        e("XKB X11 extension not supported");
        return NULL;
    }

    d("Actually retrieving keymap...");
    int32_t device_id = xkb_x11_get_core_keyboard_device_id(x11_connection);
    struct xkb_keymap *keymap = xkb_x11_keymap_new_from_device(
            context, x11_connection,
            device_id, XKB_KEYMAP_COMPILE_NO_FLAGS);
    d("Keymap has been retrieved");

    d("Disconnecting from X server...");

    xcb_disconnect(x11_connection);

    d("Disconnected from X11 server");

    return keymap;


}
#endif

#ifdef WAYLAND

typedef struct xkb_get_keymap_wayland_helper {
    struct wl_seat *seat;
    int32_t keymap_fd;
} xkb_get_keymap_wayland_helper;

static void wayland_registry_global_handler(
        void *data, struct wl_registry *registry, uint32_t id,
        const char *interface, uint32_t version) {
    xkb_get_keymap_wayland_helper *helper = data;
    d("Wayland Event: wl_registry::global (id = %d, interface = %s)", id, interface);

    if (strcmp(interface, "wl_seat") == 0)
        helper->seat = wl_registry_bind(registry, id, &wl_seat_interface, 1);
}

static void wayland_registry_global_remove_handler(
        void *data, struct wl_registry *registry, uint32_t id) {

}


static void wayland_keyboard_keymap_handler(
        void *data, struct wl_keyboard *wl_keyboard, uint32_t format,
        int32_t fd, uint32_t size) {
    xkb_get_keymap_wayland_helper *helper = data;
    d("Wayland Event: wl_keyboard::keymap (format = %u, fd = %d, size = %u)", format, fd, size);
    helper->keymap_fd = fd;
}


struct xkb_keymap * xkb_get_keymap_wayland(struct xkb_context *context) {
    /* For Wayland we have to listen to server events in order to catch the
     * wl_keyboard::keymap event which gives us the keymap file descriptor,
     * which we can use to retrieve the keymap content. */

    d("Connecting to Wayland server...");

    struct wl_display *wayland_connection = wl_display_connect(NULL);
    if (wayland_connection == NULL) {
        e("Failed to connect to Wayland server");
        return NULL;
    }

    d("Connected to Wayland server");

    struct wl_registry_listener registry_listener = {
        .global = wayland_registry_global_handler,
        .global_remove = wayland_registry_global_remove_handler
    };

    struct wl_keyboard_listener keyboard_listener = {
        .keymap = wayland_keyboard_keymap_handler
    };

    xkb_get_keymap_wayland_helper helper;
    helper.seat = NULL;

    struct wl_registry *registry = wl_display_get_registry(wayland_connection);
    wl_registry_add_listener(registry, &registry_listener, &helper);

    d("Waiting for wl_registry::global events...");

    wl_display_dispatch(wayland_connection);
    wl_display_roundtrip(wayland_connection);

    if (!helper.seat) {
        e("Failed to find Wayland seat");
        return NULL;
    }

    struct wl_keyboard * keyboard = wl_seat_get_keyboard(helper.seat);

    wl_keyboard_add_listener(keyboard, &keyboard_listener, &helper);

    d("Waiting for wl_keyboard::keymap events...");

    wl_display_dispatch(wayland_connection);
    wl_display_roundtrip(wayland_connection);

    d("Actually retrieving keymap...");

    FILE *keymap_file = fdopen(helper.keymap_fd, "rb");
    if (!keymap_file) {
        e("Failed to open keymap file");
        return NULL;
    }

    char *keymap_str = stream_read(keymap_file);

    struct xkb_keymap *keymap = xkb_keymap_new_from_string(
            context, keymap_str,
            XKB_KEYMAP_FORMAT_TEXT_V1,
            XKB_KEYMAP_COMPILE_NO_FLAGS);

    d("Keymap has been retrieved");

    d("Disconnecting from Wayland server...");

    wl_display_disconnect(wayland_connection);

    d("Disconnected from Wayland server");

    return keymap;
}

void xkb_keysyms_mapping_destroy(xkb_keysyms_mapping *mapping) {
    hash_destroy(mapping);
}

void xkb_modifiers_mapping_destroy(xkb_modifiers_mapping *mapping) {
    free(*mapping);
}

#endif