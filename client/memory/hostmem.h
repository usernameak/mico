#pragma once

#include <stddef.h>

void *micoCMRequestHostPages(size_t size);
void micoCMReleaseHostPages(void *addr);