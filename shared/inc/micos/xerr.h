#pragma once

#include <stdint.h>

typedef enum micoSXError {
    micoSX_OK             = 0,
    micoSX_BUFFER_TOO_BIG = -1,
    micoSX_MAP_FAILED     = -2,
    micoSX_OUT_OF_MEMORY  = -3,
    micoSX_SYSTEM_ERROR   = -4,
    micoSX_HOST_IO_ERROR  = -5,
} micoSXError;

#define micoSX_IS_OK(err) ((err) >= 0)