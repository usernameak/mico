#include "emu.h"

#include "../memory/hostmem.h"
#include <micos/rpc.h>

#include <unicorn/unicorn.h>
#include <micos/memory.h>

#define micoCE_PAGE_ALIGNMENT 4096

// ram starts at page 1 to trap on NULL pointer dereference
#define micoCE_RAM_ADDR 0x1000
#define micoCE_RAM_SIZE 8 * 1024 * 1024

typedef struct micoCEEmuIOBuffer {
    micoCEEmuIOBufferInfo info;

    struct micoCEEmuIOBuffer *next;
} micoCEEmuIOBuffer;

struct micoCEEmu {
    uc_engine *uc;
    micoCEEmuIOBuffer *ioBuffers;
    uint32_t nextIoGuestAddr;
    void *ram;

    char rpcBuffer[16384];
    size_t rpcRequestSize;
};

static uint64_t rpcRegReadCallback(
    uc_engine *uc, uint64_t offset,
    unsigned size, void *user_data) {

    return 0;
}

static void rpcRegWriteCallback(
    uc_engine *uc, uint64_t offset,
    unsigned size, uint64_t value,
    void *user_data) {

    micoCEEmu *emu = user_data;

    if (offset == 0) {
        emu->rpcRequestSize = value;
    } else if (offset == 4) {
        micoSRHeader *rpcHeader = (micoSRHeader *)emu->rpcBuffer;
        printf("%08x %08x\n", rpcHeader->serviceID, rpcHeader->requestType);
    }
}

micoSEError micoCEEmuCreate(micoCEEmu **outEmu) {
    micoSEError error = micoSE_OK;
    void *ram         = NULL;
    uc_engine *uc     = NULL;

    micoCEEmu *emu = malloc(sizeof(micoCEEmu));
    if (emu == NULL) return micoSE_OUT_OF_MEMORY;

    memset(emu, 0, sizeof(micoCEEmu));

    uc_err err = uc_open(UC_ARCH_ARM, UC_MODE_ARM, &uc);
    if (err != UC_ERR_OK) {
        uc    = NULL;
        error = micoSE_SYSTEM_ERROR;
        goto fail;
    }
    emu->uc = uc;

    uc_ctl_set_cpu_model(uc, UC_CPU_ARM_1136);

    emu->ioBuffers       = NULL;
    emu->nextIoGuestAddr = 0xF0000000;

    *outEmu = emu;

    uc_mmio_map(uc,
        0xF0000000, 0x10000,
        rpcRegReadCallback, emu,
        rpcRegWriteCallback, emu);

    uc_mem_map_ptr(uc,
        0xF0010000, 0x10000,
        UC_PROT_WRITE | UC_PROT_READ, emu->rpcBuffer);

    ram = micoCMRequestHostPages(micoCE_RAM_SIZE);
    if (ram == NULL) {
        error = micoSE_OUT_OF_MEMORY;
        goto fail;
    }
    emu->ram = ram;

    err = uc_mem_map_ptr(uc,
        micoCE_RAM_ADDR, micoCE_RAM_SIZE,
        UC_PROT_ALL, emu->ram);
    if (err != UC_ERR_OK) {
        fprintf(stderr, "uc_mem_map(RAM) failed: %s\n", uc_strerror(err));
        error = micoSE_MAP_FAILED;
        goto fail;
    }

    uint32_t stackPointer = micoCE_RAM_ADDR + micoCE_RAM_SIZE;

    err = uc_reg_write(uc, UC_ARM_REG_SP, &stackPointer);
    if (err != UC_ERR_OK) {
        error = micoSE_SYSTEM_ERROR;
        goto fail;
    }

    return micoSE_OK;
fail:
    if (uc != NULL) {
        uc_close(uc);
    }
    micoCMReleaseHostPages(ram);
    free(emu);
    return error;
}

void micoCEEmuDestroy(micoCEEmu *emu) {
    micoCEEmuIOBuffer *iobuf = emu->ioBuffers;
    while (iobuf) {
        micoCEEmuIOBuffer *next = iobuf->next;
        free(iobuf);
        iobuf = next;
    }

    uc_close(emu->uc);

    free(emu);
}

void *micoCEEmuGetRAM(micoCEEmu *emu) {
    return emu->ram;
}

micoSEError micoCEEmuAttachDevice(micoCEEmu *emu, micoCEDevice *dev) {
    return dev->vtbl->initialize(dev, emu);
}

micoSEError micoCEEmuMapIOBuffer(micoCEEmu *emu, size_t size, micoCEEmuIOBufferInfo *info) {
    size_t alignedSize = micoSMAlignedSize(size, micoCE_PAGE_ALIGNMENT);

    void *hostPages = micoCMRequestHostPages(alignedSize);
    if (hostPages == NULL) return micoSE_OUT_OF_MEMORY;

    uint32_t guestAddr = emu->nextIoGuestAddr - alignedSize;
    uc_err status      = uc_mem_map_ptr(emu->uc, guestAddr, alignedSize, UC_PROT_READ | UC_PROT_WRITE, guestAddr);
    if (status != UC_ERR_OK) {
        micoCMReleaseHostPages(hostPages);
        return micoSE_MAP_FAILED;
    }

    micoCEEmuIOBuffer *iobuf = malloc(sizeof(micoCEEmuIOBuffer));
    if (iobuf == NULL) {
        micoCMReleaseHostPages(hostPages);
        uc_mem_unmap(emu->uc, guestAddr, alignedSize);
        return micoSE_OUT_OF_MEMORY;
    }

    iobuf->info.guestAddr = guestAddr;
    iobuf->info.size      = alignedSize;
    iobuf->info.ptr       = hostPages;

    *info = iobuf->info;

    iobuf->next    = emu->ioBuffers;
    emu->ioBuffers = iobuf;

    return micoSE_OK;
}

micoSEError micoCEEmuBoot(micoCEEmu *emu) {
    uc_err err = uc_emu_start(emu->uc, micoCE_RAM_ADDR, 0, 0, 0);
    if (err != UC_ERR_OK) {
        return micoSE_SYSTEM_ERROR;
    }
    return micoSE_OK;
}