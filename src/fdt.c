#include <printf.h>
#include "fdt.h"

// https://stackoverflow.com/a/52653356
// These can probably be optimized now
unsigned swapb(unsigned in) { return __builtin_bswap32(in); }
unsigned long long swapb64(unsigned long long in) { return __builtin_bswap64(in); }

#define CAST(type, value) ((type)value)

void* align32(void* ptr) {
    size_t val = (size_t)ptr;
    if (val%4 != 0) {
        val = val - (val%4) + 4;
    }
    return (void*)val;
}

size_t strlen(const char* s) {
    size_t i;
    for (i = 0; s[i] != 0; i++);
    return i;
}

void print_tabs(int count) {
    for (int i = 0; i < count; i++) {
        printf("\t");
    }
}

void parse_dt_struct(char* ptr, char* strings) {
    while (1) {
        uint32_t token = swapb(*CAST(uint32_t*, ptr));
        int tabs = 0;
        ptr += 4;
        switch (token) {
            case FDT_BEGIN_NODE: {
                //size_t length = strlen(ptr);
                size_t length = sprintf(NULL, "%s", ptr);
                print_tabs(tabs);
                printf("FDT_BEGIN_NODE: %s (%zd)\n", ptr, length);
                ptr += length + 1; // Include null byte
                ptr = align32(ptr);
                tabs++;
                break;
            }
            case FDT_END_NODE: {
                tabs--;
                print_tabs(tabs);
                printf("FDT_END_NODE\n");
                break;
            }
            case FDT_PROP: {
                uint32_t len = swapb(*CAST(uint32_t*, ptr));
                ptr += 4;
                uint32_t name_offset = swapb(*CAST(uint32_t*, ptr));
                ptr += 4;
                print_tabs(tabs);
                printf("FDT_PROP: (%d) %s = ", len, &strings[name_offset]);
                for (int i = 0; i < len; i++) {
                    printf("%c", ptr[i]);
                }
                printf("\n");
                ptr += len;
                ptr = align32(ptr);
                break;
            }
            case FDT_NOP: {
                print_tabs(tabs);
                printf("FDT_NOP\n");
                break;
            }
            case FDT_END: {
                print_tabs(tabs);
                printf("FDT_END\n");
                return;
            }
            default: {
                print_tabs(tabs);
                printf("Not a token: %d\n", token);
            }
        }
    }
}

int print_fdt_info(struct fdt_header* header) {
    printf("fdt_header: %p\n", header);
    printf("fdt_header->magic: %#x\n", swapb(header->magic));
    if (swapb(header->magic) != 0xd00dfeed) {
        printf("invalid dtb header\n");
        return -1;
    }
    printf("fdt_header->totalsize: %#x\n", swapb(header->totalsize));
    printf("fdt_header->off_dt_struct: %#x\n", swapb(header->off_dt_struct));
    printf("fdt_header->off_dt_strings: %#x\n", swapb(header->off_dt_strings));
    printf("fdt_header->off_mem_rsvmap: %#x\n", swapb(header->off_mem_rsvmap));
    printf("fdt_header->version: %#x\n", swapb(header->version));
    printf("fdt_header->last_comp_version: %#x\n", swapb(header->last_comp_version));
    printf("fdt_header->boot_cpuid_phys: %#x\n", swapb(header->boot_cpuid_phys));
    printf("fdt_header->size_dt_strings: %#x\n", swapb(header->size_dt_strings));
    printf("fdt_header->size_dt_struct: %#x\n", swapb(header->size_dt_struct));

    struct fdt_reserve_entry* reserved_mem_blocks = (void*)header + swapb(header->off_mem_rsvmap);
    size_t i;
    for (i = 0; reserved_mem_blocks[i].size != 0 && reserved_mem_blocks[i].address != 0; i++) {
        printf("Memory block %zd: 0x%p-0x%p\n", i, reserved_mem_blocks[i].address, reserved_mem_blocks[i].address + reserved_mem_blocks[i].size);
    }
    if (i == 0) {
        printf("No reserved memory blocks in dtb\n");
    }

    char* dt_strings = (void*)header + swapb(header->off_dt_strings);

    char* dt_struct = (void*)header + swapb(header->off_dt_struct);
    parse_dt_struct(dt_struct, dt_strings);
    return 1;
}
