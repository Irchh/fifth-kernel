#include <csr.h>

void mstatus_set_mpp(size_t level) {
    size_t mstatus;
    csr_read("mstatus", mstatus);
    mstatus &= ~(0b11L<<11);
    mstatus |= (level&0b11)<<11;
    csr_write("mstatus", mstatus);
}
