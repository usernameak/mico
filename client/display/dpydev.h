#pragma once

#include "dpy.h"
#include "../emu/device.h"

#include <micos/err.h>

micoSEError micoCDDeviceCreate(micoCEDevice **pdev, micoCDDisplay *dpy);
