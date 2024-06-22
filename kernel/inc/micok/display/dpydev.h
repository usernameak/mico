#pragma once

#include <micos/xerr.h>

#include <micok/emu/device.h>

typedef struct micoKDFramebuffer {
    uint32_t width, height;
    unsigned char *displayBuffer;
} micoKDFramebuffer;

micoKEDevice *micoKDDeviceGetDefault();
micoSXError micoKDDeviceRequestBlit(micoKEDevice *dev_);
micoKDFramebuffer *micoKDDeviceGetFramebuffer(micoKEDevice *dev_);

extern const micoKEDeviceDriver g_micoKDDeviceDriver;
