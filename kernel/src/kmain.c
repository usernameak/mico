#include <micok/rpc.h>
#include <micos/rpc.h>
#include <micos/emu.h>
void micoKMain() {
    micoSRHeader req;
    req.serviceID   = micoSR_SERVICE_EMU;
    req.requestType = micoSE_REQ_DEVENUM;

    micoKRSend(&req, sizeof(req));

    while (1) {
        // do nothing
    }
}
