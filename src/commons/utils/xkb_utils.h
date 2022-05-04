#ifndef XKB_UTILS_H
#define XKB_UTILS_H

#include "commons/containers/hash/hash.h"
#include "xkbcommon/xkbcommon.h"

#ifndef XKB_MOD_NAME_SHIFT
#define XKB_MOD_NAME_SHIFT "Shift"
#endif

#ifndef XKB_MOD_NAME_LOCK
#define XKB_MOD_NAME_LOCK "Lock"
#endif

#ifndef XKB_MOD_NAME_CONTROL
#define XKB_MOD_NAME_CONTROL "Control"
#endif

#ifndef XKB_MOD_NAME_MOD1
#define XKB_MOD_NAME_MOD1 "Mod1"
#endif

#ifndef XKB_MOD_NAME_MOD2
#define XKB_MOD_NAME_MOD2 "Mod2"
#endif

#ifndef XKB_MOD_NAME_MOD3
#define XKB_MOD_NAME_MOD3 "Mod3"
#endif

#ifndef XKB_MOD_NAME_MOD4
#define XKB_MOD_NAME_MOD4 "Mod4"
#endif

#ifndef XKB_MOD_NAME_MOD5
#define XKB_MOD_NAME_MOD5 "Mod5"
#endif

#define XKB_MOD_INDEX_SHIFT     0
#define XKB_MOD_INDEX_LOCK      1
#define XKB_MOD_INDEX_CONTROL   2
#define XKB_MOD_INDEX_MOD1      3
#define XKB_MOD_INDEX_MOD2      4
#define XKB_MOD_INDEX_MOD3      5
#define XKB_MOD_INDEX_MOD4      6
#define XKB_MOD_INDEX_MOD5      7

#define XKB_MODIFIERS_NUM 8

#define XKB_DEFAULT_LAYOUT_INDEX 0

const char * xkb_mod_get_name(xkb_mod_index_t mod);

/*
 * MODIFIERS_MAPPING
 * xkb_keycode_t[8]
 */
typedef xkb_keycode_t * xkb_modifiers_mapping;


void xkb_modifiers_mapping_destroy(xkb_modifiers_mapping *mapping);

/*
 * KEYSYMS_MAPPING
 * xkb_keysym_t ->  xkb_keysyms_mapping_value (xkb_keycode_t keycode, xkb_mod_index_t *modifiers)
 */

typedef struct xkb_keysyms_mapping_value {
    xkb_keycode_t keycode;
    struct {
        xkb_mod_index_t *modifiers;
        size_t size;
    } modifiers;
} xkb_keysyms_mapping_value;

typedef hash xkb_keysyms_mapping;

void xkb_keysyms_mapping_destroy(xkb_keysyms_mapping *mapping);


#ifdef X11
struct xkb_keymap * xkb_get_keymap_x11(struct xkb_context *context);
#endif

#ifdef WAYLAND
struct xkb_keymap * xkb_get_keymap_wayland(struct xkb_context *context);
#endif

/* Returns the mapping between each keysym (e.g. 'A', '@') and the combination
 * of modifiers and keycode needed to generate it. */
xkb_keysyms_mapping xkb_keymap_get_keysyms_mapping(struct xkb_keymap *keymap);

/* Returns the mapping between each modifier (e.g. Shift) and the keycode
 * needed to generate it */
xkb_modifiers_mapping xkb_keymap_get_modifiers_mapping(struct xkb_keymap *keymap);



#endif // XKB_UTILS_H