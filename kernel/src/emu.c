#include <micok/emu/emu.h>

#include <micos/rpc.h>

#include <micok/rpc.h>
#include <micok/emu/device.h>

micoSXError micoKESetupDevices() {
    char respBuf[micoKR_RESPONSE_SIZE_MAX];
    size_t respSize;

    micoSRHeader req;
    req.serviceID   = micoSR_SERVICE_EMU;
    req.requestType = micoSE_REQ_DEVENUM;

    micoSXError err = micoKRSendWithResponse(&req, sizeof(req), respBuf, &respSize);
    if (!micoSX_IS_OK(err)) return err;

    micoSEDevEnumResponse *resp = (micoSEDevEnumResponse *) respBuf;
    for (uint32_t i = 0; i < resp->deviceCount; i++) {
        micoSEDevEnumRecord *edev = &resp->devices[i];

        const micoKEDeviceDriver *driver = micoKEFindDeviceDriver(edev->deviceTypeID);
        micoKEDevice *device = driver->deviceVtbl->CreateDevice(driver, edev);
    }

    return micoSX_OK;
}
