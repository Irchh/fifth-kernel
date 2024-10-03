#include "priv.h"
#include "csr.h"

void set_priv_m(size_t level, void* entry_func) {
    mstatus_set_mpp(level);

    size_t mepc = (size_t)entry_func;
    csr_write("mepc", mepc);

    size_t satp = 0;
    csr_write("satp", satp);

    size_t pmpaddr0 = 0x3fffffffffffffL;
    size_t pmpcfg0 = 0xf;
    csr_write("pmpaddr0", pmpaddr0);
    csr_write("pmpcfg0", pmpcfg0);
    __asm__("mret");
    while (1);
}
