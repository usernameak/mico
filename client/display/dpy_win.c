#include "dpy.h"
#include <windows.h>

#define micoCD_WIN_WINDOW_CLASS L"micoClientDisplay"

struct micoCDDisplay {
    HWND hwnd;
    int width;
    int height;
};

static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

micoCDDisplay *micoCDCreateDisplay(int width, int height) {
    static int isClassRegistered = 0;
    RECT wrc = { 0, 0, width, height };

    micoCDDisplay *display = malloc(sizeof(micoCDDisplay));
    if (display == NULL) goto fail;

    display->width = width;
    display->height = height;

    if (!isClassRegistered) {
        WNDCLASSEXW wcls;
        ZeroMemory(&wcls, sizeof(wcls));
        wcls.cbSize        = sizeof(wcls);
        wcls.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wcls.lpfnWndProc   = &WndProc;
        wcls.hInstance     = GetModuleHandleW(NULL);
        wcls.hCursor       = LoadCursorA(NULL, IDC_ARROW);
        wcls.hbrBackground = GetStockObject(BLACK_BRUSH);
        wcls.lpszClassName = micoCD_WIN_WINDOW_CLASS;

        RegisterClassExW(&wcls);

        isClassRegistered = 1;
    }

    AdjustWindowRectEx(&wrc, WS_OVERLAPPEDWINDOW, FALSE, 0);

    display->hwnd = CreateWindowExW(
        0,
        micoCD_WIN_WINDOW_CLASS,
        L"micoClient Display",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        wrc.right - wrc.left,
        wrc.bottom - wrc.top,
        NULL,
        NULL,
        GetModuleHandleW(NULL),
        NULL);
    if (display->hwnd == NULL) goto fail;

    ShowWindow(display->hwnd, SW_SHOW);

    return display;

fail:
    free(display);
    return NULL;
}

void micoCDDestroyDisplay(micoCDDisplay *display) {
    if (display->hwnd != NULL) {
        DestroyWindow(display->hwnd);
    }
    free(display);
}

void micoCDGetDimensions(micoCDDisplay *dpy, int *pwidth, int *pheight) {
    *pwidth = dpy->width;
    *pheight = dpy->height;
}
