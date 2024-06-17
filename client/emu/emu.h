#pragma once

#include "device.h"


#include <stddef.h>
#include <stdint.h>

#include <micos/xerr.h>

typedef struct micoCEEmuIOBufferInfo {
    void *ptr;
    size_t size;
    uint32_t guestAddr;
} micoCEEmuIOBufferInfo;

typedef struct micoCEEmu micoCEEmu;

micoSXError micoCEEmuCreate(micoCEEmu **outEmu);
void micoCEEmuDestroy(micoCEEmu *emu);

void *micoCEEmuGetRAM(micoCEEmu *emu);

micoSXError micoCEEmuAttachDevice(micoCEEmu *emu, micoCEDevice *dev);
micoSXError micoCEEmuMapIOBuffer(micoCEEmu *emu, size_t size, micoCEEmuIOBufferInfo *info);

micoSXError micoCEEmuBoot(micoCEEmu *emu);
