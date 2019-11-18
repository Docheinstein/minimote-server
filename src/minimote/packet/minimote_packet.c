#include "minimote_packet.h"
#include "commons/utils/byte_utils.h"
#include <stdio.h>


// Returns the difference between the real packet byte count and the expected byte count.
// Thus returns 0 if the packet has the expected length
int minimote_packet_parse(minimote_packet *packet, byte * bytes, int bytecount) {
    packet->expected_packet_length = bytes[0];
    packet->event_type = bytes[1];
    packet->event_time = bytes_to_uint64(bytes) & 0xFFFFFFFFFFFFFU;
    packet->payload = &bytes[MINIMOTE_PACKET_HEADER_SIZE];

   return bytecount - packet->expected_packet_length;
}

void minimote_packet_dump(minimote_packet *packet) {
    printf("-- packet_type: %s (%d)\n",
            minimote_packet_type_to_string(packet->event_type), packet->event_type);
    printf("-- event_time: %lu\n", packet->event_time);
    printf("-- payload: ");

    char bits[9];

    for (int i = 0; i < packet->expected_packet_length - MINIMOTE_PACKET_HEADER_SIZE; i++) {
        printf("%s | ", byte_to_bin(packet->payload[i], bits));
    }

    printf("\n");
}
