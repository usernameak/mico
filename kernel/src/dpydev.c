#include "micos/display.h"


#include <micok/display/dpydev.h>

#include <stdlib.h>

#include <micok/rpc.h>
#include <micok/emu/device.h>

typedef struct micoKDDevice {
    const micoKEDeviceVtbl *vtbl;
    uint32_t serviceID;
    uint32_t width, height;
    unsigned char *displayBuffer;
} micoKDDevice;

static const micoKEDeviceVtbl g_micoKDDeviceVtbl;

micoKEDevice *micoKDDeviceCreate(const micoKEDeviceDriver *driver, const micoSEDevEnumRecord *enumRecord) {
    micoKDDevice *device = malloc(sizeof(micoKDDevice));
    if (device == NULL) {
        return NULL;
    }
    device->vtbl = &g_micoKDDeviceVtbl;

    device->serviceID = enumRecord->serviceID;

    char respBuf[micoKR_RESPONSE_SIZE_MAX];
    size_t respSize;

    micoSRHeader req;
    req.serviceID   = enumRecord->serviceID;
    req.requestType = micoSD_REQ_QUERY_INFO;

    micoSXError err = micoKRSendWithResponse(&req, sizeof(req), &respBuf, &respSize);
    if (!micoSX_IS_OK(err)) {
        free(device);
        return NULL;
    }

    micoSDDisplayInfo *dpyinfo = (micoSDDisplayInfo *)respBuf;
    device->width              = dpyinfo->width;
    device->height             = dpyinfo->height;
    device->displayBuffer      = (unsigned char *)dpyinfo->guestBufferPtr;

    for (int y = 0; y < device->height; y++) {
        for (int x = 0; x < device->width; x++) {
            device->displayBuffer[(y * device->width + x) * 3] = 255;
            device->displayBuffer[(y * device->width + x) * 3 + 1] = 0;
            device->displayBuffer[(y * device->width + x) * 3 + 2] = 255;
        }
    }

    micoSRHeader reqBlit;
    reqBlit.serviceID   = enumRecord->serviceID;
    reqBlit.requestType = micoSD_REQ_REFRESH_DISPLAY;
    micoKRSend(&reqBlit, sizeof(reqBlit));

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
