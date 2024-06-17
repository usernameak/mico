#pragma once

#include "device.h"


#include <stddef.h>
#include <stdint.h>

#include <micos/err.h>

typedef struct micoCEEmuIOBufferInfo {
    void *ptr;
    size_t size;
    uint32_t guestAddr;
} micoCEEmuIOBufferInfo;

typedef struct micoCEEmu micoCEEmu;

micoSEError micoCEEmuCreate(micoCEEmu **outEmu);
void micoCEEmuDestroy(micoCEEmu *emu);

void *micoCEEmuGetRAM(micoCEEmu *emu);

micoSEError micoCEEmuAttachDevice(micoCEEmu *emu, micoCEDevice *dev);
micoSEError micoCEEmuMapIOBuffer(micoCEEmu *emu, size_t size, micoCEEmuIOBufferInfo *info);

micoSEError micoCEEmuBoot(micoCEEmu *emu);
