#ifndef BYTE_UTILS_H
#define BYTE_UTILS_H

#include "commons/globals.h"

char * byte_to_bin(byte B, char * bits);
uint64 bytes_to_uint64(const byte *bytes);
uint32 bytes_to_uint32(const byte *bytes);
uint8 bytes_to_uint8(const byte *bytes);

#endif // BYTE_UTILS_H
