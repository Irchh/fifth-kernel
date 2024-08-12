#include <stddef.h>

#define csr_read(csr, dst) __asm__ (\
    "csrr %0, "csr\
    : "=r" (dst)\
)

#define csr_write(csr, val) __asm__ (\
    "csrw "csr", %0"\
    : : "r" (val)\
)

__attribute__((always_inline))
size_t get_mxlen(size_t misa) {
    return misa>>(sizeof(size_t)*8-2);
}
