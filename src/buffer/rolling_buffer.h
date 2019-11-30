#ifndef ROLLING_BUFFER_H
#define ROLLING_BUFFER_H

#include "commons/globals.h"

typedef struct rolling_buffer_t {
    byte *buffer;
    uint32 capacity;
    uint32 start;
    uint32 end;
} rolling_buffer;

void rolling_buffer_init(rolling_buffer *rbuffer, uint32 capacity);
bool rolling_buffer_push(rolling_buffer *rbuffer, byte *data, uint32 data_length);

byte * rolling_buffer_get(rolling_buffer *rbuffer);
void rolling_buffer_advance(rolling_buffer *rbuffer, uint32 count);
uint32 rolling_buffer_current_length(rolling_buffer *rbuffer);

#endif // ROLLING_BUFFER_H
