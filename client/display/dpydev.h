#pragma once

#include "dpy.h"
#include "../emu/device.h"

#include <micos/xerr.h>

micoSXError micoCDDeviceCreate(micoCEDevice **pdev, micoCDDisplay *dpy);
