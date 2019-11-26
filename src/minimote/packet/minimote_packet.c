#include "minimote_packet.h"
#include "commons/utils/byte_utils.h"
#include <stdio.h>
#include <commons/utils/time_utils.h>
#include <stdlib.h>
#include <string.h>

// Returns the difference between the real packet byte count and the expected byte count.
// Thus returns 0 if the packet has the expected length
int minimote_packet_parse(minimote_packet *packet, byte * bytes, int bytecount) {
    // Default values
    packet->expected_packet_length = 0;
    packet->event_type = NONE;
    packet->event_time = 0;
    packet->payload = NULL;
    packet->valid = false;

    if (bytecount == 0)
        return 0;

    // Packet length
    packet->expected_packet_length = bytes[0];
    int bytes_surplus = bytecount - packet->expected_packet_length;

    if (packet->expected_packet_length <= 1)
        return bytes_surplus;

    // Event type
    packet->event_type = bytes[1];

    if (packet->expected_packet_length < MINIMOTE_PACKET_HEADER_SIZE)
        return bytes_surplus;

    // Event time
    packet->event_time = bytes_to_uint64(bytes) & 0xFFFFFFFFFFFFU;

    if (packet->expected_packet_length < MINIMOTE_PACKET_HEADER_SIZE + 1)
        return bytes_surplus;

    // Payload
    packet->payload = &bytes[MINIMOTE_PACKET_HEADER_SIZE];

    packet->valid = true;

    return bytecount - packet->expected_packet_length;
}

void minimote_packet_dump(minimote_packet *packet) {
    printf("-- valid: %d\n", packet->valid);
    printf("-- packet_length: %d\n", packet->expected_packet_length);
    printf("-- packet_type: %s (%d)\n",
            minimote_packet_type_to_string(packet->event_type),
            packet->event_type);
    printf("-- event_time: %lu\n", packet->event_time);

    if (packet->valid) {

        char bin[1024];
        printf("-- payload %s\n",
                bytes_to_bin_pretty(
                        (byte *) packet->payload,
                        packet->expected_packet_length - MINIMOTE_PACKET_HEADER_SIZE,
                        bin));
    }

    printf("\n");
}

void minimote_packet_data(minimote_packet *packet, byte * out_bytes) {
    uint64 type = ((uint64) packet->event_type) & 0xFF;
    uint64 len = ((uint64) packet->expected_packet_length) & 0xFF;
    uint64 uid = current_ms();
    uint64 header =
        (len << 56) |
        (type << 48) |
        (uid & 0xFFFFFFFFFFFFFU);

    char bin[1024];
    printf("minimote_packet_data header: %s\n", bytes_to_bin_pretty((byte *) &header, 8, bin));


    put_uint64(out_bytes, 0, header);
//    memcpy(out_bytes, &header, 8);
    memcpy(&out_bytes[8], packet->payload, packet->expected_packet_length - MINIMOTE_PACKET_HEADER_SIZE);

    printf("minimote_packet_data: %s\n", bytes_to_bin_pretty(out_bytes, packet->expected_packet_length, bin));
}
