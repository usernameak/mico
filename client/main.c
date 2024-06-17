#include <stdio.h>
#include <unicorn/unicorn.h>

#include "display/dpy.h"
#include "emu/emu.h"
#include "emu/loader.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: micoclient <micokernel.bin>\n");
        return 1;
    }

    const char *kernel_filename = argv[1];

    micoCDDisplay *disp = micoCDCreateDisplay(240, 320);

    micoCEEmu *emu;
    micoSEError err = micoCEEmuCreate(&emu);
    if (!micoSE_IS_OK(err)) {
        fprintf(stderr, "micoCEEmuCreate failed\n");
        return 1;
    }

    micoCELoaderLoadKernel(emu, kernel_filename);

    err = micoCEEmuBoot(emu);
    if (err != micoSE_OK) {
        fprintf(stderr, "boot failed\n");
        return 1;
    }

    micoCEEmuDestroy(emu);

    micoCDDestroyDisplay(disp);

    return 0;
}
