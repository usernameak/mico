#include "emu.h"
#include "micos/emu.h"

#include "../memory/hostmem.h"
#include "../os/event.h"

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

typedef struct micoCEEmuDevice {
    uint32_t deviceTypeID;
    uint32_t serviceID;
    micoCEDevice *device;

    struct micoCEEmuDevice *next;
} micoCEEmuDevice;

struct micoCEEmu {
    uc_engine *uc;

    micoCEEmuIOBuffer *ioBuffers;
    uint32_t nextIoGuestAddr;

    micoCEEmuDevice *devices;
    uint32_t nextDevServiceID;
    uint32_t deviceCount;

    void *ram;

    char rpcBuffer[16384];
    size_t rpcRequestSize;
};

static uint64_t rpcRegReadCallback(
    uc_engine *uc, uint64_t offset,
    unsigned size, void *user_data) {

    micoCEEmu *emu = user_data;

    if (offset == 0) {
        return emu->rpcRequestSize;
    }

    return 0;
}

static void rpcServiceEmuRequest(micoCEEmu *emu, uint32_t requestType) {
    if (requestType == micoSE_REQ_DEVENUM) {
        micoSEDevEnumResponse *resp = (micoSEDevEnumResponse *)&emu->rpcBuffer[0];

        resp->deviceCount = emu->deviceCount;

        micoSEDevEnumRecord *record = resp->devices;
        micoCEEmuDevice *edev       = emu->devices;
        while (edev) {
            record->serviceID    = edev->serviceID;
            record->deviceTypeID = edev->deviceTypeID;

            edev = edev->next;
            record++;
        }

        emu->rpcRequestSize = (char *)record - emu->rpcBuffer;
    }
}

static void rpcRegWriteCallback(
    uc_engine *uc, uint64_t offset,
    unsigned size, uint64_t value,
    void *user_data) {

    micoCEEmu *emu = user_data;

    if (offset == 0) {
        emu->rpcRequestSize = value;
    } else if (offset == 4) {
        if (value == 1) { // rpc send
            micoSRHeader *rpcHeader = (micoSRHeader *)emu->rpcBuffer;
            if (rpcHeader->serviceID == micoSR_SERVICE_EMU) {
                rpcServiceEmuRequest(emu, rpcHeader->requestType);
            } else {
                micoCEEmuDevice *dev = emu->devices;
                while (dev) {
                    if (rpcHeader->serviceID == dev->serviceID) {
                        dev->device->vtbl->ServiceRPC(
                            dev->device,
                            rpcHeader->requestType,
                            emu->rpcBuffer,
                            &emu->rpcRequestSize);
                    }
                    dev = dev->next;
                }
            }
        } else if (value == 2) { // release to event loop
            micoCSEventWait();
        }
    }
}

static bool memFailCallback(uc_engine *uc, uc_mem_type type,
    uint64_t address, int size, int64_t value,
    void *user_data) {
    return false;
}

micoSXError micoCEEmuCreate(micoCEEmu **outEmu) {
    micoSXError error = micoSX_OK;
    void *ram         = NULL;
    uc_engine *uc     = NULL;

    micoCEEmu *emu = malloc(sizeof(micoCEEmu));
    if (emu == NULL) return micoSX_OUT_OF_MEMORY;

    memset(emu, 0, sizeof(micoCEEmu));

    uc_err err = uc_open(UC_ARCH_ARM, UC_MODE_ARM, &uc);
    if (err != UC_ERR_OK) {
        uc    = NULL;
        error = micoSX_SYSTEM_ERROR;
        goto fail;
    }
    emu->uc = uc;

    uc_ctl_set_cpu_model(uc, UC_CPU_ARM_1136);

    emu->ioBuffers       = NULL;
    emu->nextIoGuestAddr = 0xF0000000;

    emu->devices          = NULL;
    emu->nextDevServiceID = 0x80000000;
    emu->deviceCount      = 0;

    *outEmu = emu;

    uc_hook unmappedHook;
    err = uc_hook_add(uc, &unmappedHook, UC_HOOK_MEM_UNMAPPED, &memFailCallback, emu, 0, UINT64_MAX);
    if (err != UC_ERR_OK) {
        fprintf(stderr, "uc_hook_add(UC_HOOK_MEM_UNMAPPED) failed: %s\n", uc_strerror(err));
        error = micoSX_SYSTEM_ERROR;
        goto fail;
    }

    uc_mmio_map(uc,
        0xF0000000, 0x10000,
        rpcRegReadCallback, emu,
        rpcRegWriteCallback, emu);

    uc_mem_map_ptr(uc,
        0xF0010000, 0x10000,
        UC_PROT_WRITE | UC_PROT_READ, emu->rpcBuffer);

    ram = micoCMRequestHostPages(micoCE_RAM_SIZE);
    if (ram == NULL) {
        error = micoSX_OUT_OF_MEMORY;
        goto fail;
    }
    emu->ram = ram;

    err = uc_mem_map_ptr(uc,
        micoCE_RAM_ADDR, micoCE_RAM_SIZE,
        UC_PROT_ALL, emu->ram);
    if (err != UC_ERR_OK) {
        fprintf(stderr, "uc_mem_map(RAM) failed: %s\n", uc_strerror(err));
        error = micoSX_MAP_FAILED;
        goto fail;
    }

    uint32_t stackPointer = micoCE_RAM_ADDR + micoCE_RAM_SIZE;

    err = uc_reg_write(uc, UC_ARM_REG_SP, &stackPointer);
    if (err != UC_ERR_OK) {
        error = micoSX_SYSTEM_ERROR;
        goto fail;
    }

    return micoSX_OK;
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

    micoCEEmuDevice *dev = emu->devices;
    while (dev) {
        micoCEEmuDevice *next = dev->next;
        free(dev);
        dev = next;
    }

    uc_close(emu->uc);

    free(emu);
}

void *micoCEEmuGetRAM(micoCEEmu *emu) {
    return emu->ram;
}

micoSXError micoCEEmuAttachDevice(micoCEEmu *emu, micoCEDevice *dev) {
    micoSXError err = dev->vtbl->Initialize(dev, emu);
    if (!micoSX_IS_OK(err)) return err;

    micoCEEmuDevice *edev = malloc(sizeof(micoCEEmuDevice));
    if (edev == NULL) {
        // TODO: we cannot deinitialize yet.
        return micoSX_OUT_OF_MEMORY;
    }

    edev->deviceTypeID = dev->vtbl->QueryDeviceTypeID(dev);
    edev->device       = dev;
    edev->serviceID    = emu->nextDevServiceID++;

    edev->next   = emu->devices;
    emu->devices = edev;
    emu->deviceCount++;

    return micoSX_OK;
}

micoSXError micoCEEmuMapIOBuffer(micoCEEmu *emu, size_t size, micoCEEmuIOBufferInfo *info) {
    size_t alignedSize = micoSMAlignedSize(size, micoCE_PAGE_ALIGNMENT);

    void *hostPages = micoCMRequestHostPages(alignedSize);
    if (hostPages == NULL) return micoSX_OUT_OF_MEMORY;

    uint32_t guestAddr = emu->nextIoGuestAddr - alignedSize;
    uc_err status      = uc_mem_map_ptr(emu->uc, guestAddr, alignedSize, UC_PROT_READ | UC_PROT_WRITE, hostPages);
    if (status != UC_ERR_OK) {
        micoCMReleaseHostPages(hostPages);
        return micoSX_MAP_FAILED;
    }

    micoCEEmuIOBuffer *iobuf = malloc(sizeof(micoCEEmuIOBuffer));
    if (iobuf == NULL) {
        micoCMReleaseHostPages(hostPages);
        uc_mem_unmap(emu->uc, guestAddr, alignedSize);
        return micoSX_OUT_OF_MEMORY;
    }

    iobuf->info.guestAddr = guestAddr;
    iobuf->info.size      = alignedSize;
    iobuf->info.ptr       = hostPages;

    *info = iobuf->info;

    iobuf->next    = emu->ioBuffers;
    emu->ioBuffers = iobuf;

    return micoSX_OK;
}

micoSXError micoCEEmuBoot(micoCEEmu *emu) {
    uc_err err = uc_emu_start(emu->uc, micoCE_RAM_ADDR, 0, 0, 0);
    if (err != UC_ERR_OK) {
        return micoSX_SYSTEM_ERROR;
    }
    return micoSX_OK;
}
