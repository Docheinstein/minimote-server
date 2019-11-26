#ifndef BYTE_UTILS_H
#define BYTE_UTILS_H

#include "commons/globals.h"

const char * byte_to_bin(byte B, char * bin);
const char * bytes_to_bin(byte *Bs, int count, char * bin);
const char * bytes_to_bin_pretty(byte *Bs, int count, char * bin);

uint64 bytes_to_uint64(const byte *bytes);
uint32 bytes_to_uint32(const byte *bytes);
uint8 bytes_to_uint8(const byte *bytes);

void put_uint64(byte *bytes, int offset, uint64 val);

#endif // BYTE_UTILS_H
