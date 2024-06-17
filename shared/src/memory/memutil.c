#include <stddef.h>

#include <micos/memory.h>

size_t micoSMAlignedSize(size_t size, size_t alignment) {
    if (size % alignment == 0) return size;
    return size + (alignment - (size % alignment));
}
