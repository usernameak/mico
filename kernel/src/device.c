#include <stddef.h>
#include <micok/emu/device.h>

#include <micok/display/dpydev.h>

const micoKEDeviceDriver *g_micoKERegistredDeviceDrivers[] = {
    &g_micoKDDeviceDriver,
    NULL
};

const micoKEDeviceDriver *micoKEFindDeviceDriver(uint32_t deviceTypeID) {
    const micoKEDeviceDriver **ppDriver = g_micoKERegistredDeviceDrivers;
    while (*ppDriver) {
        if ((*ppDriver)->deviceTypeID == deviceTypeID) {
            return *ppDriver;
        }
        ppDriver++;
    }
    return NULL;
}
