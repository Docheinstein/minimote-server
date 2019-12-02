#include "rolling_buffer.h"
#include <stdlib.h>
#include <string.h>
#include "logging/logging.h"

static void rolling_buffer_do_roll(rolling_buffer *rbuffer);

void rolling_buffer_init(rolling_buffer *rbuffer, uint32 capacity) {
    rbuffer->capacity = capacity;
    rbuffer->buffer = malloc(sizeof(byte) * capacity);
    rbuffer->start = rbuffer->end = 0;
    bzero(rbuffer->buffer, capacity);
}

bool rolling_buffer_push(rolling_buffer *rbuffer, byte *data, uint32 data_length) {
    if (rbuffer->end + data_length >= rbuffer->capacity) {
        // Not enough space, perform a roll
        rolling_buffer_do_roll(rbuffer);

        // Not enough space even after the roll, doing nothing
        if (rbuffer->end + data_length >= rbuffer->capacity) {
            w("Buffer full! Current: %d, Maximum: %d, Needed: %d",
              rolling_buffer_current_length(rbuffer), rbuffer->capacity, rbuffer->end + data_length);
            return false;
        }
    }

    // Push at the end and advance the cursor
    memcpy(&rbuffer->buffer[rbuffer->end], data, data_length);
    rbuffer->end += data_length;

    return true;
}

uint32 rolling_buffer_current_length(rolling_buffer *rbuffer) {
    return rbuffer->end - rbuffer->start;
}

byte *rolling_buffer_get(rolling_buffer *rbuffer) {
    return &rbuffer->buffer[rbuffer->start];
}

void rolling_buffer_advance(rolling_buffer *rbuffer, uint32 count) {
    rbuffer->start += count;
}

void rolling_buffer_destroy(rolling_buffer *rbuffer) {
    free(rbuffer->buffer);
}

void rolling_buffer_do_roll(rolling_buffer *rbuffer) {
    // The current situation should be something like this
    // | -- garbabe ------------------------ (START) ---- (END) -- |
    // And will become
    // (START) ---- (END) -----------------------------------------|

    uint32 current_filled_buffer_size = rolling_buffer_current_length(rbuffer);

    // Copy the filled buffer part, being aware that overlap might occur: memmove
    memmove(rbuffer->buffer, &rbuffer->buffer[rbuffer->start], current_filled_buffer_size);

    rbuffer->end = current_filled_buffer_size;
    rbuffer->start = 0;
}
