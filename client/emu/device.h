#pragma once

#include <micos/err.h>

typedef struct micoCEDeviceVtbl micoCEDeviceVtbl;

typedef struct micoCEEmu micoCEEmu;

typedef struct micoCEDevice {
    const micoCEDeviceVtbl *vtbl;
} micoCEDevice;

struct micoCEDeviceVtbl {
    void (*destroy)(micoCEDevice *dev);
    micoSEError (*initialize)(micoCEDevice *dev, micoCEEmu *emu);
};
