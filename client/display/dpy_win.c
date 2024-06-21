#include "dpy.h"
#include <windows.h>
#include <assert.h>

#define micoCD_WIN_WINDOW_CLASS L"micoClientDisplay"

struct micoCDDisplay {
    HWND hwnd;
    int width;
    int height;
    void *displayBuffer;
};

static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    micoCDDisplay *dpy = NULL;

    if (uMsg == WM_NCCREATE) {
        LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;

        dpy       = (micoCDDisplay *)lpcs->lpCreateParams;
        dpy->hwnd = hwnd;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)dpy);
    } else {
        dpy = (micoCDDisplay *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (uMsg == WM_PAINT) {
        PAINTSTRUCT ps;
        ZeroMemory(&ps, sizeof(ps));

        HDC hdc = BeginPaint(hwnd, &ps);

        assert(ps.rcPaint.left >= 0);
        assert(ps.rcPaint.top >= 0);
        assert(ps.rcPaint.right <= dpy->width);
        assert(ps.rcPaint.bottom <= dpy->height);

        BITMAPINFOHEADER bmi;
        ZeroMemory(&bmi, sizeof(bmi));
        bmi.biSize          = sizeof(bmi);
        bmi.biWidth         = dpy->width;
        bmi.biHeight        = -dpy->height;
        bmi.biPlanes        = 1;
        bmi.biBitCount      = 24;
        bmi.biCompression   = BI_RGB;
        bmi.biXPelsPerMeter = 3780;
        bmi.biYPelsPerMeter = 3780;
        bmi.biClrUsed       = 0;
        bmi.biClrImportant  = 0;

        SetDIBitsToDevice(
            hdc,
            ps.rcPaint.left,
            ps.rcPaint.top,
            ps.rcPaint.right - ps.rcPaint.left,
            ps.rcPaint.bottom - ps.rcPaint.top,
            ps.rcPaint.left,
            ps.rcPaint.top,
            0,
            dpy->height,
            dpy->displayBuffer,
            (BITMAPINFO *)&bmi,
            DIB_RGB_COLORS);

        EndPaint(hwnd, &ps);

        return 0;
    }

    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

micoCDDisplay *micoCDCreateDisplay(int width, int height) {
    static int isClassRegistered = 0;
    RECT wrc                     = { 0, 0, width, height };

    micoCDDisplay *display = malloc(sizeof(micoCDDisplay));
    if (display == NULL) goto fail;

    display->hwnd          = NULL;
    display->width         = width;
    display->height        = height;
    display->displayBuffer = NULL;

    display->displayBuffer = malloc(width * height * 3);
    if (display->displayBuffer == NULL) goto fail;
    memset(display->displayBuffer, 0, width * height * 3);

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

    DWORD wndStyle = WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX);

    AdjustWindowRectEx(&wrc, wndStyle, FALSE, 0);

    display->hwnd = CreateWindowExW(
        0,
        micoCD_WIN_WINDOW_CLASS,
        L"micoClient Display",
        wndStyle,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        wrc.right - wrc.left,
        wrc.bottom - wrc.top,
        NULL,
        NULL,
        GetModuleHandleW(NULL),
        display);
    if (display->hwnd == NULL) goto fail;

    ShowWindow(display->hwnd, SW_SHOW);

    return display;

fail:
    if (display) {
        if (display->hwnd != NULL) {
            DestroyWindow(display->hwnd);
        }
        free(display->displayBuffer);
        free(display);
    }
    return NULL;
}

void micoCDDestroyDisplay(micoCDDisplay *display) {
    if (display->hwnd != NULL) {
        DestroyWindow(display->hwnd);
    }
    free(display);
}

void micoCDGetDimensions(micoCDDisplay *dpy, int *pwidth, int *pheight) {
    *pwidth  = dpy->width;
    *pheight = dpy->height;
}

void micoCDBlitBuffer(micoCDDisplay *dpy, const void *buffer) {
    memcpy(dpy->displayBuffer, buffer, dpy->width * dpy->height * 3);
    InvalidateRect(dpy->hwnd, NULL, FALSE);
}
