#pragma once

#include <micos/xerr.h>

typedef struct micoCEDeviceVtbl micoCEDeviceVtbl;

typedef struct micoCEEmu micoCEEmu;

typedef struct micoCEDevice {
    const micoCEDeviceVtbl *vtbl;
} micoCEDevice;

struct micoCEDeviceVtbl {
    void (*Destroy)(micoCEDevice *dev);
    micoSXError (*Initialize)(micoCEDevice *dev, micoCEEmu *emu);

    uint32_t (*QueryDeviceTypeID)(micoCEDevice *dev);
    micoSXError (*ServiceRPC)(
        micoCEDevice *dev,
        uint32_t requestType,
        void *rpcBuffer,
        size_t *bufferSize);
};
