#include <micok/rpc.h>

#include <stdint.h>
#include <string.h>

#define RPC_REGISTER_BASE 0xF0000000

static volatile uint32_t *const rpcSendSizeRegister = (volatile uint32_t *)(RPC_REGISTER_BASE + 0);
static volatile uint32_t *const rpcSendRegister     = (volatile uint32_t *)(RPC_REGISTER_BASE + 4);

// TODO: make sure compiler cannot reorder non-volatile writes across volatile ones
static void *const rpcSendBuf = (void *)0xF0010000;

#define RPC_SEND_BUF_SIZE 0x10000

micoKEError micoKRSend(const void *buf, size_t size) {
    if (size > RPC_SEND_BUF_SIZE) {
        // TODO: segment request and remove buffer limit later
        return micoKE_BUFFER_TOO_BIG;
    }

    memcpy(rpcSendBuf, buf, size);
    *rpcSendSizeRegister = size;
    *rpcSendRegister     = 0x1;

    return micoKE_OK;
}
