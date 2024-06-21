#pragma once

#include <stdint.h>

typedef struct micoSDDisplayInfo {
    uint32_t width, height;
    uint32_t guestBufferPtr;
} micoSDDisplayInfo;

#define micoSD_REQ_QUERY_INFO 0x00000001
#define micoSD_REQ_REFRESH_DISPLAY 0x00000002
