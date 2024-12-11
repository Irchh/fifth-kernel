#include "page.h"
#include "printf.h"
#include "fdt.h"
#include "frame.h"
#include "mem.h"
#include <csr.h>

pte_rv39_t* allocate_new_page_table() {
    void* pt = (void*)(device_information.ram_start + 4096*allocate_first_frame());
    //printf("Page table frame: %#zx\n", pt);
    memset(pt, 0, 4096);
    return pt;
}

void create_ident_map_page_table(pte_rv39_t* table) {
    for (size_t i = 0; i < 512; i++) {
        table[i].r = 1;
        table[i].w = 1;
        table[i].x = 1;
        table[i].v = 1;
        table[i].ppn = MAP_GIGAPAGE_ALIGN*i;
    }
}

int map_page(pte_rv39_t* table, size_t ppn, size_t vpn, map_t type) {
    if (type == MAP_GIGAPAGE && (ppn % MAP_GIGAPAGE_ALIGN != 0 || vpn % MAP_GIGAPAGE_ALIGN != 0)) {
        return ERR_ALIGN;
    }
    if (type == MAP_MEGAPAGE && (ppn % MAP_MEGAPAGE_ALIGN != 0 || vpn % MAP_MEGAPAGE_ALIGN != 0)) {
        return ERR_ALIGN;
    }

    int ppn_arr[3];
    ppn_arr[0] = ppn&0x1FF;
    ppn_arr[1] = (ppn>>9)&0x1FF;
    ppn_arr[2] = (ppn>>18)&0x1FF;

    int vpn_arr[3];
    vpn_arr[0] = vpn&0x1FF;
    vpn_arr[1] = (vpn>>9)&0x1FF;
    vpn_arr[2] = (vpn>>18)&0x1FF;

    for (map_t i = type - 1; i > 0; i--) {
        // Check if the next table is allocated
        //      If not, allocate it
        // Go to next table and repeat until last table reached
        if (table[vpn_arr[i]].v == 0) {
            table[vpn_arr[i]].v = 1;
            table[vpn_arr[i]].r = 0;
            table[vpn_arr[i]].w = 0;
            table[vpn_arr[i]].x = 0;
            table[vpn_arr[i]].ppn = ((size_t)allocate_new_page_table()/4096);
        } else if (table[vpn_arr[i]].r || table[vpn_arr[i]].w || table[vpn_arr[i]].x) {
            // Already mapped as part of bigger page
            return ERR_ALREADY_MAPPED;
        }
        // TODO: Check if the mapped page is valid (if bits are extended? check risc-v pdf)
        // TODO: Make sure the ppn is mapped to current address space, and translate it to that mapped address
        table = (void*)(table[vpn_arr[i]].ppn*4096);
    }

    // Map the page
    pte_rv39_t* entry = &table[vpn_arr[3-type]];
    if (entry->v) {
        return ERR_ALREADY_MAPPED;
    }
    entry->r = 1;
    entry->w = 1;
    entry->x = 1;
    entry->v = 1;
    entry->ppn = ppn;

    return 0;
}

extern size_t kernel_start_addr;
extern size_t kernel_end_addr;

void enable_paging() {
    pte_rv39_t* pt = allocate_new_page_table();

    size_t kernel_start_page = kernel_start_addr/4096;
    size_t kernel_end_page = (kernel_end_addr+4095)/4096;

    for (size_t page = kernel_start_page; page <= kernel_end_page; page++) {
        map_page(pt, page, page, MAP_PAGE);
    }
    
    // Map memory mapped devices like uart, etc.
    for (int i = 0; i < sizeof(device_information.mapped_locations)/sizeof(device_information.mapped_locations[0]); i++) {
        if (device_information.mapped_locations[i].address == 0 && device_information.mapped_locations[i].size == 0)
            continue;
        size_t start_page = device_information.mapped_locations[i].address / 4096;
        size_t end_page = (device_information.mapped_locations[i].address + device_information.mapped_locations[i].size + 4095) / 4096;
        // TODO: Map larger pages for larger areas
        for (size_t page = start_page; page <= end_page; page++)
            map_page(pt, page, page, MAP_PAGE);
    }

    size_t satp;
    csr_write("satp", ((size_t)pt)/4096 | PAGING_MODE_SV39);
    csr_read("satp", satp);
    printf("satp: %#zx\n", satp);
}
