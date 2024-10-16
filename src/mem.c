#include <stddef.h>

size_t page_of(size_t addr) {
    return addr>>12;
}

size_t addr_of(size_t page) {
    return page<<12;
}

void* memset(void* dest, int c, size_t count) {
    char* _dest = dest;
    for (size_t i = 0; i < count; i++)
        (*_dest++) = c;
    return dest;
}

void* memcpy(void* dest, const void* src, size_t count) {
    for (size_t i = 0; i < count; i++) {
        ((char*)dest)[i] = ((char*)src)[i];
    }
    return dest;
}
