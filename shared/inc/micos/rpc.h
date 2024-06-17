#pragma once

#include <stdint.h>

typedef struct micoSRHeader {
    uint32_t serviceID;
    uint32_t requestType;
} micoSRHeader;

#define micoSR_SERVICE_EMU 0x00000001
