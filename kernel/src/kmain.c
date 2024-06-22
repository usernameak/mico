#include <micok/rpc.h>
#include <micok/version.h>
#include <micok/display/dpytext.h>

#include <micok/emu/emu.h>

__attribute__((noreturn)) void micoKMain() {
    micoKESetupDevices();

    micoKDTextDraw("micoKernel VERSION " micoK_VERSION "\n");
    while (1) {
        micoKREventWait();
    }
}
