#include "typing_utils.h"
#include <malloc.h>
#include <xkbcommon/xkbcommon.h>

char *unicode_to_string(uint32 unicode) {
    char *name = malloc(32 * sizeof(char));
    xkb_keysym_get_name(xkb_utf32_to_keysym(unicode), name, 32);
    return name;
}
