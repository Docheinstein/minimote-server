#include <sys/types.h>
#include "byte_utils.h"

const char * byte_to_bin_str(byte B, char * bin) {
    uint i = 0;
    for (i = 0; i < 8; i++)
        bin[7 - i] = (B & (1u << i)) ? '1' : '0';
    bin[i] = '\0';
    return bin;
}

const char * bytes_to_bin_str(byte *Bs, int count, char *bin) {
    for (uint i = 0; i < count; i++)
        byte_to_bin_str(Bs[i], &bin[i * 8]);
    return bin;
}

const char * bytes_to_bin_str_pretty(byte *Bs, int count, char *bin) {
    static const int BYTE_PRINT_SIZE = 11;
    uint i;
    for (i = 0; i < count; i++) {
        uint pos = i * BYTE_PRINT_SIZE;
        byte_to_bin_str(Bs[i], &bin[pos]);
        bin[pos + 8] = ' ';
        bin[pos + 9] = '|';
        bin[pos + 10] = ' ';
    }
    bin[i * BYTE_PRINT_SIZE] = '\0';

    return bin;
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

void put_uint64(byte *bytes, int offset, uint64 val) {
    bytes[offset + 0] = (val >> 56) & 0xFF;
    bytes[offset + 1] = (val >> 48) & 0xFF;
    bytes[offset + 2] = (val >> 40) & 0xFF;
    bytes[offset + 3] = (val >> 32) & 0xFF;
    bytes[offset + 4] = (val >> 24) & 0xFF;
    bytes[offset + 5] = (val >> 16) & 0xFF;
    bytes[offset + 6] = (val >> 8) & 0xFF;
    bytes[offset + 7] = (val) & 0xFF;
}

void put_uint32(byte *bytes, int offset, uint32 val) {
    bytes[offset + 0] = (val >> 24) & 0xFF;
    bytes[offset + 1] = (val >> 16) & 0xFF;
    bytes[offset + 2] = (val >> 8) & 0xFF;
    bytes[offset + 3] = (val) & 0xFF;
}
