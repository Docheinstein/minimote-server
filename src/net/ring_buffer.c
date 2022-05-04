#include "ring_buffer.h"
#include <stdlib.h>
#include <string.h>
#include "log/log.h"

static void ring_buffer_roll(ring_buffer *buffer) {
    // The current situation should be something like this
    // | -- garbage ------------------------ (START) ---- (END) -- |
    // And will become
    // (START) ---- (END) -----------------------------------------|

    uint32 current_filled_buffer_size = ring_buffer_current_length(buffer);

    // Copy the filled buffer part, using memmove to handle overlaps
    memmove(buffer->buffer, &buffer->buffer[buffer->start], current_filled_buffer_size);

    buffer->end = current_filled_buffer_size;
    buffer->start = 0;
}

void ring_buffer_init(ring_buffer *buffer, uint32 capacity) {
    buffer->capacity = capacity;
    buffer->buffer = malloc(capacity * sizeof(byte));
    buffer->start = buffer->end = 0;
    bzero(buffer->buffer, capacity);
}

void ring_buffer_destroy(ring_buffer *buffer) {
    free(buffer->buffer);
}

bool ring_buffer_push(ring_buffer *buffer, byte *data, uint32 data_length) {
    if (buffer->end + data_length >= buffer->capacity) {
        // Not enough space, perform a roll
        ring_buffer_roll(buffer);

        // Not enough space even after the roll, doing nothing
        if (buffer->end + data_length >= buffer->capacity) {
            w("ring buffer is full! current: %d, capacity: %d, needed: %d",
              ring_buffer_current_length(buffer), buffer->capacity, buffer->end + data_length);
            return false;
        }
    }

    // Push at the end and advance the cursor
    memcpy(&buffer->buffer[buffer->end], data, data_length);
    buffer->end += data_length;

    return true;
}

uint32 ring_buffer_current_length(ring_buffer *buffer) {
    return buffer->end - buffer->start;
}

byte *ring_buffer_get(ring_buffer *buffer) {
    return &buffer->buffer[buffer->start];
}

void ring_buffer_advance(ring_buffer *buffer, uint32 count) {
    buffer->start += count;
}