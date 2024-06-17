#include <micok/rpc.h>
#include <micos/rpc.h>

#define micoKMTestServiceID 0x00000001
#define micoKMTestRequestType 0x00000001

struct micoKMTestRequest {
    micoSRHeader header;
};

void micoKMain() {
    struct micoKMTestRequest req;
    req.header.serviceID   = micoKMTestServiceID;
    req.header.requestType = micoKMTestRequestType;

    micoKRSend(&req, sizeof(req));

    while (1) {
        // do nothing
    }
}
