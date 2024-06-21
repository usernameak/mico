#include "micok/rpc.h"

#include <micok/emu/emu.h>

__attribute__((noreturn))
void micoKMainThumb() {
    micoKESetupDevices();

    while (1) {
        micoKREventWait();
    }
}

__attribute__((target("arm"), noreturn))
void micoKMain() {
    micoKMainThumb();
}
