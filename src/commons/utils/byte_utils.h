#ifndef BYTE_UTILS_H
#define BYTE_UTILS_H

#include "commons/types.h"

const char * byte_to_bin_str(byte B, char * bin);
const char * bytes_to_bin_str(byte *Bs, int count, char * bin);
const char * bytes_to_bin_str_pretty(byte *Bs, int count, char * bin);

uint64 bytes_to_uint64(const byte *bytes);
uint32 bytes_to_uint32(const byte *bytes);
uint16 bytes_to_uint16(const byte *bytes);
uint8 bytes_to_uint8(const byte *bytes);

void put_uint64(byte *bytes, int offset, uint64 val);
void put_uint32(byte *bytes, int offset, uint32 val);

#endif // BYTE_UTILS_H
