#ifndef MINIMOTE_UTILS_H
#define MINIMOTE_UTILS_H

#include "minimote_defs.h"

char * byte_to_bin(byte B, char * bits);
uint64 bytes_to_uint64(const byte *bytes);
uint32 bytes_to_uint32(const byte *bytes);

#endif // MINIMOTE_UTILS_H
