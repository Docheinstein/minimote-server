#include <sys/types.h>
#include "byte_utils.h"

char * byte_to_bin(byte B, char * bits) {
    for (uint i = 0; i < 8; i++)
        bits[7 - i] = (B & (1u << i)) ? '1' : '0';
    bits[8] = '\0';

    return bits;
}

uint64 bytes_to_uint64(const byte *bytes) {
    return (uint64) (
        (uint64) bytes[0] << 56U |
        (uint64) bytes[1] << 48U |
        (uint64) bytes[2] << 40U |
        (uint64) bytes[3] << 32U |
        (uint64) bytes[4] << 24U |
        (uint64) bytes[5] << 16U |
        (uint64) bytes[6] << 8U |
        (uint64) bytes[7]);

}

uint32 bytes_to_uint32(const byte *bytes) {
    return (uint32) (
        (uint32) bytes[0] << 24 |
        (uint32) bytes[1] << 16 |
        (uint32) bytes[2] << 8 |
        (uint32) bytes[3]);

}

uint8 bytes_to_uint8(const byte *bytes) {
    return (uint32) bytes[0];
}
