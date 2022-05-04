#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include "commons/types.h"

// Ring buffer
typedef struct ring_buffer {
    byte *buffer;
    uint32 capacity;
    uint32 start;
    uint32 end;
} ring_buffer;

void ring_buffer_init(ring_buffer *buffer, uint32 capacity);
void ring_buffer_destroy(ring_buffer *buffer);

bool ring_buffer_push(ring_buffer *buffer, byte *data, uint32 data_length);

byte * ring_buffer_get(ring_buffer *buffer);
void ring_buffer_advance(ring_buffer *buffer, uint32 count);
uint32 ring_buffer_current_length(ring_buffer *buffer);

#endif // RING_BUFFER_H