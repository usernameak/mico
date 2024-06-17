#pragma once

#include <stddef.h>

#include <micos/xerr.h>

#define micoKR_RESPONSE_SIZE_MAX 0x10000

micoSXError micoKRSend(const void *buf, size_t size);
micoSXError micoKRSendWithResponse(
    const void *buf, size_t size,
    void *responseBuf, size_t *responseSize);
