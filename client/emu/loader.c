#include "loader.h"

#include <stdio.h>

micoSXError micoCELoaderLoadKernel(micoCEEmu *emu, const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        return micoSX_HOST_IO_ERROR;
    }
    if (0 != fseek(fp, 0, SEEK_END)) {
        fclose(fp);
        return micoSX_HOST_IO_ERROR;
    }
    long size = ftell(fp);
    if (size < 0) {
        fclose(fp);
        return micoSX_HOST_IO_ERROR;
    }
    if (0 != fseek(fp, 0, SEEK_SET)) {
        fclose(fp);
        return micoSX_HOST_IO_ERROR;
    }

    void *ram = micoCEEmuGetRAM(emu);
    if (size != fread(ram, 1, size, fp)) {
        fclose(fp);
        return 1;
    }

    fclose(fp);

    return micoSX_OK;
}
