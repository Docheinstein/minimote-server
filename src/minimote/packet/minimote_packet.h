#ifndef MINIMOTE_PACKET_H
#define MINIMOTE_PACKET_H

#include "commons/globals.h"
#include "type/minimote_packet_type.h"

#define MINIMOTE_PACKET_HEADER_SIZE 8
#define MINIMOTE_PACKET_MINIMUM_SIZE MINIMOTE_PACKET_HEADER_SIZE

typedef struct minimote_packet_t {
    minimote_packet_type event_type;
    uint8 expected_packet_length;
    uint64 event_time;
    byte *payload;
} minimote_packet;

int minimote_packet_parse(minimote_packet *packet, byte * bytes, int bytecount);
void minimote_packet_dump(minimote_packet *packet);

#endif // MINIMOTE_PACKET_H
