#include "page.h"
#include "printf.h"
#include "fdt.h"
#include "frame.h"
#include <csr.h>

size_t ident_map_addr;

pte_rv39_t* current_pt() {
    size_t satp;
    csr_read("satp", satp);
    return (void*)(satp*4096 + ident_map_addr);
}

pte_rv39_t* allocate_new_page_table() {
    void* pt = (void*)(device_information.ram_start + 4096*allocate_first_frame() + ident_map_addr);
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

    if (table == nullptr) {
        table = current_pt();
        if (table == nullptr) {
            return ERR_NULL_PT;
        }
    }

    size_t vpn_arr[3];
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
            table[vpn_arr[i]].ppn = ((size_t)allocate_new_page_table() - ident_map_addr)/4096;
        } else if (table[vpn_arr[i]].r || table[vpn_arr[i]].w || table[vpn_arr[i]].x) {
            // Already mapped as part of bigger page
            return ERR_ALREADY_MAPPED;
        }
        // TODO: Check if the mapped page is valid (if bits are extended? check risc-v pdf)
        // TODO: Make sure the ppn is mapped to current address space, and translate it to that mapped address
        table = (void*)(table[vpn_arr[i]].ppn*4096 + ident_map_addr);
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

    __asm__ __volatile__ (
        "sfence.vma x0, x0"
    );

    return 0;
}

int allocate_a_page(size_t *ppn, size_t *vpn) {
    const auto frame = first_free_frame();
    const auto allocated_ppn = frame_to_phys_addr(allocate_frame(frame))/4096;
    for (size_t i = 0x80000; i <= 0x100000; i++) {
        const auto res = map_page(nullptr, allocated_ppn, i, MAP_PAGE);
        if (res == 0) {
            *ppn = allocated_ppn;
            *vpn = i;
            return 0;
        }
    }
    *ppn = 0;
    *vpn = 0;
    deallocate_frame(frame);
    return ERR_UNABLE_TO_MAP;
}

// TODO: Figure out if the table pointer should be a physical pointer or not. For now it is assumed that it is already correct for the current address space
int unmap_page(pte_rv39_t* table, size_t vpn, map_t type) {
    if (type == MAP_GIGAPAGE && vpn % MAP_GIGAPAGE_ALIGN != 0) {
        return ERR_ALIGN;
    }
    if (type == MAP_MEGAPAGE && vpn % MAP_MEGAPAGE_ALIGN != 0) {
        return ERR_ALIGN;
    }

    if (table == nullptr) {
        table = current_pt();
        if (table == nullptr) {
            return ERR_NULL_PT;
        }
    }

    size_t vpn_arr[3];
    vpn_arr[0] = vpn&0x1FF;
    vpn_arr[1] = (vpn>>9)&0x1FF;
    vpn_arr[2] = (vpn>>18)&0x1FF;

    for (map_t i = type - 1; i > 0; i--) {
        if (table[vpn_arr[i]].v == 0) {
            return ERR_NOT_MAPPED;
        }
        if (table[vpn_arr[i]].r || table[vpn_arr[i]].w || table[vpn_arr[i]].x) {
            // Trying to unmap a page with the incorrect page size,
            // e.g. trying to unmap a normal 4k page but the allocated page is a megapage
            return ERR_INCORRECT_MAP_TYPE;
        }
        table = (void*)(table[vpn_arr[i]].ppn*4096+ident_map_addr);
    }

    pte_rv39_t* entry = &table[vpn_arr[3-type]];
    if (entry->v == 0) {
        return ERR_NOT_MAPPED;
    }
    memset(entry, 0, sizeof(pte_rv39_t));
    __asm__ __volatile__ (
        "sfence.vma x0, x0"
    );
    // TODO: check if entire page table is empty, and deallocate it
    return 0;
}

void identity_map_memory(pte_rv39_t* table, size_t page_offset) {
    for (size_t i = device_information.ram_start/4096; i < (device_information.ram_start+device_information.ram_size)/4096; i++) {
        map_page(table, i, page_offset + i, MAP_PAGE);
    }
}

void unmap_lower_half_kernel() {
    pte_rv39_t* pt = current_pt();

    size_t kernel_start_page = kernel_start_addr/4096;
    size_t kernel_end_page = (kernel_end_addr+4095)/4096;
    for (size_t page = kernel_start_page; page <= kernel_end_page; page++) {
        // Keep currently needed pages as well, since we don't jump yet
        unmap_page(pt, page, MAP_PAGE);
    }
}

// Should only be called once at boot to transition to virtual memory
void enable_paging() {
    ident_map_addr = 0;
    pte_rv39_t* pt = allocate_new_page_table();

    size_t kernel_start_page = kernel_start_addr/4096;
    size_t kernel_end_page = (kernel_end_addr+4095)/4096;

    // Map kernel pages
    for (size_t page = kernel_start_page; page <= kernel_end_page; page++) {
        // Keep currently needed pages as well, since we don't jump yet
        map_page(pt, page, page, MAP_PAGE);
        // Higher-half kernel maps
        map_page(pt, page, HIGHER_HALF_VPN+page-kernel_start_page, MAP_PAGE);
    }

    // Map memory mapped devices like uart, etc.
    for (size_t i = 0; i < sizeof(device_information.mapped_locations)/sizeof(device_information.mapped_locations[0]); i++) {
        if (device_information.mapped_locations[i].address == 0 && device_information.mapped_locations[i].size == 0)
            continue;
        size_t start_page = device_information.mapped_locations[i].address / 4096;
        size_t end_page = (device_information.mapped_locations[i].address + device_information.mapped_locations[i].size + 4095) / 4096;
        // TODO: Map larger pages for larger areas, if possible
        for (size_t page = start_page; page <= end_page; page++)
            map_page(pt, page, page, MAP_PAGE);
    }

    // Identity map memory to higher half address + 4GiB
    identity_map_memory(pt, HIGHER_HALF_VPN+1024*1024);

    size_t satp;
    csr_write("satp", ((size_t)pt)/4096 | PAGING_MODE_SV39);
    csr_read("satp", satp);
    printf("satp: %#zx\n", satp);
    ident_map_addr = IDENT_MAP_ADDR;
}
