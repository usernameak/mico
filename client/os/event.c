#include "event.h"

#include <windows.h>

void micoCSEventWait() {
    MSG msg;
    BOOL bRet = GetMessageW(&msg, NULL, 0, 0);
    if (bRet == -1 || bRet == 0) return;

    TranslateMessage(&msg);
    DispatchMessageW(&msg);
}

