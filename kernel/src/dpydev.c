#include <micok/display/dpydev.h>

#include <stdlib.h>

#include <micos/display.h>

#include <micok/rpc.h>
#include <micok/emu/device.h>

typedef struct micoKDDevice {
    const micoKEDeviceVtbl *vtbl;
    uint32_t serviceID;
    micoKDFramebuffer fb;
} micoKDDevice;

static const micoKEDeviceVtbl g_micoKDDeviceVtbl;
static micoKEDevice *g_defaultDisplayDevice = NULL;

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
    device->fb.width           = dpyinfo->width;
    device->fb.height          = dpyinfo->height;
    device->fb.displayBuffer   = (unsigned char *)dpyinfo->guestBufferPtr;

    if (g_defaultDisplayDevice == NULL) {
        g_defaultDisplayDevice = (micoKEDevice *)device;
    }

    return (micoKEDevice *)device;
}

micoKDFramebuffer *micoKDDeviceGetFramebuffer(micoKEDevice *dev_) {
    micoKDDevice *dev = (micoKDDevice *)dev_;
    return &dev->fb;
}

micoSXError micoKDDeviceRequestBlit(micoKEDevice *dev_) {
    micoKDDevice *dev = (micoKDDevice *)dev_;

    micoSRHeader reqBlit;
    reqBlit.serviceID   = dev->serviceID;
    reqBlit.requestType = micoSD_REQ_REFRESH_DISPLAY;
    return micoKRSend(&reqBlit, sizeof(reqBlit));
}

micoKEDevice *micoKDDeviceGetDefault() {
    return g_defaultDisplayDevice;
}

void micoKDDeviceDestroy(micoKEDevice *device) {
    if (g_defaultDisplayDevice == device) {
        g_defaultDisplayDevice = NULL;
    }
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
