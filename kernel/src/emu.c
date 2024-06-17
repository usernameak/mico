#include <micok/emu.h>

#include <micos/rpc.h>
#include <micok/rpc.h>

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

    }

    return micoSX_OK;
}
