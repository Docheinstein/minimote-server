#include "minimote_packet.h"
#include <stdio.h>
#include <string.h>
#include "commons/utils/byte_utils.h"
#include "commons/utils/time_utils.h"
#include "log/log.h"

#define MINIMOTE_PACKET_PARSE_FAILED ~((int) 0)

#define MINIMOTE_PACKET_TIME_MASK 0xFFFFFFFFFFFFU

void minimote_packet_init(minimote_packet *packet) {
    packet->packet_length = 0;
    packet->packet_type = NONE;
    packet->event_time = 0;
    packet->payload = NULL;
}

// Returns the difference between the real packet byte count and the expected byte count.
// Thus returns
// negative number: the bytecount is lower than the expected packet length
// 0:               the packet has the expected length
// positive number: the bytecount is greater than the expected packet length
int minimote_packet_parse(minimote_packet *packet, byte *bytes, int bytecount) {
    // Default values
    minimote_packet_init(packet);

    if (bytecount < MINIMOTE_PACKET_MINIMUM_SIZE)
        return MINIMOTE_PACKET_PARSE_FAILED;

    packet->packet_length = bytes[0];

    if (packet->packet_length < MINIMOTE_PACKET_MINIMUM_SIZE) {
        w("Unexpected packet length %d, should be at least %d",
                packet->packet_length, MINIMOTE_PACKET_MINIMUM_SIZE);
        return MINIMOTE_PACKET_PARSE_FAILED;
    }

    int bytes_surplus = bytecount - packet->packet_length;

    // Event type
    packet->packet_type = bytes[1];

    // Event time
    packet->event_time = bytes_to_uint64(bytes) & MINIMOTE_PACKET_TIME_MASK;

    // Payload (eventually)
    if (packet->packet_length > MINIMOTE_PACKET_HEADER_SIZE) {
        packet->payload = &bytes[MINIMOTE_PACKET_HEADER_SIZE];
    }

    return bytes_surplus;
}

void minimote_packet_dump(minimote_packet *packet) {
    bool is_valid =  minimote_packet_is_valid(packet);
    d("-- valid: %s", (is_valid ? "yes" : "no"));
    d("-- packet_length: %d", packet->packet_length);
    d("-- packet_type: %s (%d)",
            minimote_packet_type_to_string(packet->packet_type),
            packet->packet_type);
    d("-- event_time: %lu", packet->event_time);

    if (is_valid) {
        if (packet->payload) {
            char payload_bin_str[1024];
            d("-- payload %s",
              bytes_to_bin_str_pretty(
                      (byte *) packet->payload,
                      minimote_packet_payload_length(packet),
                      payload_bin_str));
        }
    }
}

void minimote_packet_data(minimote_packet *packet, byte *out_bytes) {
    uint64 type = ((uint64) packet->packet_type) & 0xFF;
    uint64 len = ((uint64) packet->packet_length) & 0xFF;
    uint64 uid = ms();
    uint64 header =
        (len << 56) |
        (type << 48) |
        (uid & MINIMOTE_PACKET_TIME_MASK);

#if LOG_DEBUG
    char bin_str[1024];
#endif

    d("minimote_packet_data header: %s",
            bytes_to_bin_str_pretty((byte *) &header, 8, bin_str));

    // Push header
    put_uint64(out_bytes, 0, header);

    // Push payload (if any)
    if (packet->payload) {
        memcpy(&out_bytes[8], packet->payload, minimote_packet_payload_length(packet));
    }

    d("minimote_packet_data: %s",
            bytes_to_bin_str_pretty(out_bytes, packet->packet_length, bin_str));
}

bool minimote_packet_is_valid(minimote_packet *packet) {
    return
        packet &&
        packet->packet_length >= MINIMOTE_PACKET_MINIMUM_SIZE &&
        packet->packet_type != NONE;
}

int minimote_packet_payload_length(minimote_packet *packet) {
    return packet->packet_length - MINIMOTE_PACKET_HEADER_SIZE;
}