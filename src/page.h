#ifndef PAGE_H
#define PAGE_H

#define PAGING_MODE_BARE 0b0000L<<60
#define PAGING_MODE_SV39 0b1000L<<60
#define PAGING_MODE_SV48 0b1001L<<60
#define PAGING_MODE_SV57 0b1010L<<60
#define PAGING_MODE_SV64 0b1011L<<60

#define MAP_PAGE     0b11
#define MAP_MEGAPAGE 0b10
#define MAP_GIGAPAGE 0b01

#define MAP_MEGAPAGE_ALIGN 512
#define MAP_GIGAPAGE_ALIGN 262144

#define ERR_ALIGN -1
#define ERR_ALREADY_MAPPED -2
#define ERR_NOT_MAPPED -3
#define ERR_INCORRECT_MAP_TYPE -4

#define HIGHER_HALF_ADDR_START 0xFFFFFFC000000000L
#define HIGHER_HALF_VPN 0x4000000L
#define IDENT_MAP_ADDR HIGHER_HALF_ADDR_START+(4L*1024*1024*1024)

typedef unsigned int map_t;

typedef struct __attribute__((packed)) {
    unsigned int v:1;
    unsigned int r:1;
    unsigned int w:1;
    unsigned int x:1;
    unsigned int u:1;
    unsigned int g:1;
    unsigned int a:1;
    unsigned int d:1;
    unsigned int rsw:2;
    unsigned long int ppn:44;
    unsigned int reserved:7;
    unsigned int pbmt:2;
    unsigned int n:1;
} pte_rv39_t;

void enable_paging();
void unmap_lower_half_kernel();

#endif
