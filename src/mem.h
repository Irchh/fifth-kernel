#include <stddef.h>

size_t page_of(size_t addr);
size_t addr_of(size_t page);

void* memset(void *ptr, int c, size_t n);
void* memcpy(void* dest, const void* src, size_t count);
