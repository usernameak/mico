#include "dpydev.h"

#include "../emu/emu.h"

#include <stdlib.h>

typedef struct micoCDDevice {
    const struct micoCEDeviceVtbl *vtbl;

    micoCDDisplay *dpy;
    micoCEEmuIOBufferInfo dpyBufferInfo;
} micoCDDevice;

static const micoCEDeviceVtbl g_micoCDDeviceVtbl;

micoSEError micoCDDeviceCreate(micoCEDevice **pdev, micoCDDisplay *dpy) {
    micoCDDevice *dev = malloc(sizeof(micoCDDevice));
    if (dev == NULL) {
        return micoSE_OUT_OF_MEMORY;
    }

    dev->vtbl = &g_micoCDDeviceVtbl;
    dev->dpy  = dpy;

    *pdev = (micoCEDevice *)dev;

    return micoSE_OK;
}

static void micoCDDeviceDestroy(micoCEDevice *dev) {
    free(dev);
}

static micoSEError micoCDDeviceInitialize(micoCEDevice *dev_, micoCEEmu *emu) {
    micoCDDevice *dev = (micoCDDevice *)dev_;

    int width, height;
    micoCDGetDimensions(dev->dpy, &width, &height);

    return micoCEEmuMapIOBuffer(emu, width * height * 3, &dev->dpyBufferInfo);
}

static const micoCEDeviceVtbl g_micoCDDeviceVtbl = {
    &micoCDDeviceDestroy,
    &micoCDDeviceInitialize
};
