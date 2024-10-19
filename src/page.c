#include "page.h"
#include "printf.h"
#include "fdt.h"
#include "frame.h"
#include "mem.h"
#include <csr.h>

void create_ident_map_page_table(pte_rv39_t* table) {
    printf("Size of pte_rv39_t: %zd\n", sizeof(*table));
    for (size_t i = 0; i < 512; i++) {
        table[i].r = 1;
        table[i].w = 1;
        table[i].x = 1;
        table[i].v = 1;
        table[i].ppn = 262144*i;
    }
}

void enable_paging() {
    size_t frame = device_information.ram_start + 4096*allocate_first_frame();
    printf("Page table frame: %#zx\n", frame);
    memset((char*)(frame), 0, 4096);

    create_ident_map_page_table((void*) frame);

    size_t satp;
    csr_write("satp", frame/4096 | PAGING_MODE_SV39);
    csr_read("satp", satp);
    printf("satp: %#zx\n", satp);
}
