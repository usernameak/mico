#include "dpydev.h"

#include "../emu/emu.h"
#include <micos/emu.h>

#include <stdlib.h>

typedef struct micoCDDevice {
    const struct micoCEDeviceVtbl *vtbl;

    micoCDDisplay *dpy;
    micoCEEmuIOBufferInfo dpyBufferInfo;
} micoCDDevice;

static const micoCEDeviceVtbl g_micoCDDeviceVtbl;

micoSXError micoCDDeviceCreate(micoCEDevice **pdev, micoCDDisplay *dpy) {
    micoCDDevice *dev = malloc(sizeof(micoCDDevice));
    if (dev == NULL) {
        return micoSX_OUT_OF_MEMORY;
    }

    dev->vtbl = &g_micoCDDeviceVtbl;
    dev->dpy  = dpy;

    *pdev = (micoCEDevice *)dev;

    return micoSX_OK;
}

static void micoCDDeviceDestroy(micoCEDevice *dev) {
    free(dev);
}

static micoSXError micoCDDeviceInitialize(micoCEDevice *dev_, micoCEEmu *emu) {
    micoCDDevice *dev = (micoCDDevice *)dev_;

    int width, height;
    micoCDGetDimensions(dev->dpy, &width, &height);

    return micoCEEmuMapIOBuffer(emu, width * height * 3, &dev->dpyBufferInfo);
}


static uint32_t micoCDDeviceQueryDeviceTypeID(micoCEDevice *dev) {
    return micoSE_DEVICE_TYPE_DISPLAY;
}

static const micoCEDeviceVtbl g_micoCDDeviceVtbl = {
    &micoCDDeviceDestroy,
    &micoCDDeviceInitialize,
    &micoCDDeviceQueryDeviceTypeID
};
