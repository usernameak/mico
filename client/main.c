#include <stdio.h>
#include <unicorn/unicorn.h>

#include "display/dpy.h"
#include "display/dpydev.h"
#include "emu/emu.h"
#include "emu/loader.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: micoclient <micokernel.bin>\n");
        return 1;
    }

    const char *kernel_filename = argv[1];

    micoCDDisplay *disp = micoCDCreateDisplay(240, 320);
    micoCEDevice *dispDev;
    micoSXError err = micoCDDeviceCreate(&dispDev, disp);
    if (!micoSX_IS_OK(err)) {
        fprintf(stderr, "micoCDDeviceCreate failed\n");
        return 1;
    }

    micoCEEmu *emu;
    err = micoCEEmuCreate(&emu);
    if (!micoSX_IS_OK(err)) {
        fprintf(stderr, "micoCEEmuCreate failed\n");
        return 1;
    }

    err = micoCEEmuAttachDevice(emu, dispDev);
    if (!micoSX_IS_OK(err)) {
        fprintf(stderr, "micoCEEmuAttachDevice failed\n");
        return 1;
    }

    micoCELoaderLoadKernel(emu, kernel_filename);

    err = micoCEEmuBoot(emu);
    if (err != micoSX_OK) {
        fprintf(stderr, "boot failed\n");
        return 1;
    }

    micoCEEmuDestroy(emu);

    micoCDDestroyDisplay(disp);

    return 0;
}
