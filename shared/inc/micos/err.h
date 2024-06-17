#pragma once

#include <stdint.h>

typedef enum micoSEError {
    micoSE_OK             = 0,
    micoSE_BUFFER_TOO_BIG = -1,
    micoSE_MAP_FAILED     = -2,
    micoSE_OUT_OF_MEMORY  = -3,
    micoSE_SYSTEM_ERROR   = -4,
    micoSE_HOST_IO_ERROR  = -5,
} micoSEError;

#define micoSE_IS_OK(err) ((err) >= 0)