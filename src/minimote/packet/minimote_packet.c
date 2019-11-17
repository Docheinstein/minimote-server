#include "minimote_packet.h"
#include "commons/utils/byte_utils.h"
#include <stdio.h>

#define HEADER_SIZE 8

void minimote_packet_parse(minimote_packet *packet, byte * bytes, int bytecount) {
    packet->event_type = bytes[0];
    packet->event_time = bytes_to_uint64(bytes) & 0xFFFFFFFFFFFFFFU;
    packet->payload = &bytes[HEADER_SIZE];
    packet->payload_length = bytecount - HEADER_SIZE;
}

void minimote_packet_dump(minimote_packet *packet) {
    printf("-- packet_type: %s (%d)\n",
            minimote_packet_type_to_string(packet->event_type), packet->event_type);
    printf("-- event_time: %lu\n", packet->event_time);
    printf("-- payload: ");

    char bits[9];

    for (int i = 0; i < packet->payload_length; i++) {
        printf("%s | ", byte_to_bin(packet->payload[i], bits));
    }

    printf("\n");
}
