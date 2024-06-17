#include "micok/emu/device.h"

#include <micok/display/dpydev.h>
#include <stdlib.h>

typedef struct micoKDDevice {
    const micoKEDeviceVtbl *vtbl;
} micoKDDevice;

static const micoKEDeviceVtbl g_micoKDDeviceVtbl;

micoKEDevice *micoKDDeviceCreate(const micoKEDeviceDriver *driver, const micoSEDevEnumRecord *enumRecord) {
    micoKDDevice *device = malloc(sizeof(micoKDDevice));
    if (device == NULL) {
        return NULL;
    }
    device->vtbl = &g_micoKDDeviceVtbl;
    return (micoKEDevice *)device;
}

void micoKDDeviceDestroy(micoKEDevice *device) {
    free(device);
}

static const micoKEDeviceVtbl g_micoKDDeviceVtbl = {
    &micoKDDeviceCreate,
    &micoKDDeviceDestroy
};

const micoKEDeviceDriver g_micoKDDeviceDriver = {
    &g_micoKDDeviceVtbl,
    micoSE_DEVICE_TYPE_DISPLAY
};
