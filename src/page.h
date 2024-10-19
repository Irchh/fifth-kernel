#ifndef PAGE_H
#define PAGE_H

#define PAGING_MODE_BARE 0b0000L<<60
#define PAGING_MODE_SV39 0b1000L<<60
#define PAGING_MODE_SV48 0b1001L<<60
#define PAGING_MODE_SV57 0b1010L<<60
#define PAGING_MODE_SV64 0b1011L<<60

typedef struct __attribute__((packed)) {
    unsigned int v:1;
    unsigned int r:1;
    unsigned int w:1;
    unsigned int x:1;
    unsigned int u:1;
    unsigned int g:1;
    unsigned int a:1;
    unsigned int d:1;
    unsigned int rsv:2;
    unsigned long int ppn:44;
    unsigned int reserved:7;
    unsigned int pbmt:2;
    unsigned int n:1;
} pte_rv39_t;

void enable_paging();

#endif
