#include <micok/emu/emu.h>

__attribute__((noreturn))
void micoKMainThumb() {
    micoKESetupDevices();

    while (1) {
        // do nothing
    }
}

__attribute__((target("arm"), noreturn))
void micoKMain() {
    micoKMainThumb();
}
