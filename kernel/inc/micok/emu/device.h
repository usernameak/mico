#pragma once

#include <micos/emu.h>
#include <stdint.h>

typedef struct micoKEDevice micoKEDevice;
typedef struct micoKEDeviceDriver micoKEDeviceDriver;

typedef struct micoKEDeviceVtbl {
    micoKEDevice *(*CreateDevice)(const micoKEDeviceDriver *driver, const micoSEDevEnumRecord *enumRecord);
    void (*DestroyDevice)(micoKEDevice *device);
} micoKEDeviceVtbl;

struct micoKEDevice {
    const micoKEDeviceVtbl *deviceVtbl;
};

struct micoKEDeviceDriver {
    const micoKEDeviceVtbl *deviceVtbl;
    uint32_t deviceTypeID;
};

extern const micoKEDeviceDriver *g_micoKERegistredDeviceDrivers[];

const micoKEDeviceDriver *micoKEFindDeviceDriver(uint32_t deviceTypeID);
