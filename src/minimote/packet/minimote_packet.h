#ifndef MINIMOTE_PACKET_H
#define MINIMOTE_PACKET_H

#include "commons/globals.h"
#include "type/minimote_packet_type.h"

typedef struct minimote_packet_t {
    minimote_packet_type event_type;
    uint64 event_time;
    byte *payload;
    int payload_length;
} minimote_packet;

void minimote_packet_parse(minimote_packet *packet, byte * bytes, int bytecount);
void minimote_packet_dump(minimote_packet *packet);

#endif // MINIMOTE_PACKET_H
