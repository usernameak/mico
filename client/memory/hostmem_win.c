#include "hostmem.h"

#include <windows.h>

void *micoCMRequestHostPages(size_t size) {
    void *mem = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    return mem;
}

void micoCMReleaseHostPages(void *addr) {
    if (addr == NULL) return;
    VirtualFree(addr, 0, MEM_RELEASE);
}
