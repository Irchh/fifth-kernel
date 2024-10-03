#include <stddef.h>

#define csr_read(csr, dst) __asm__ (\
    "csrr %0, "csr\
    : "=r" (dst)\
)

#define csr_write(csr, val) __asm__ (\
    "csrw "csr", %0"\
    : : "r" (val)\
)

void mstatus_set_mpp(size_t level);
