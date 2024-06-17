#pragma once

#include <stdint.h>

#define micoSE_DEVICE_TYPE_DISPLAY 0x00000001

typedef struct micoSEDevEnumRecord {
    uint32_t deviceTypeID;
    uint32_t serviceID;
} micoSEDevEnumRecord;

typedef struct micoSEDevEnumResponse {
    uint32_t deviceCount;
    micoSEDevEnumRecord devices[];
} micoSEDevEnumResponse;

#define micoSE_REQ_DEVENUM 0x00000001
