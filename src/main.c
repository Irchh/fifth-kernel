#include <stdint.h>
#include <printf.h>
#include <csr.h>
#include "trap.h"
#include "fdt.h"

extern long long add(long long a, long long b);
extern unsigned long long counter;

int kernel_main(uint64_t _idk, struct fdt_header* dtb) {
    long long i = add(4, 5);
    printf("%d", i);
    printf("Hello, rv64!\n");

    size_t misa;
    csr_read("misa", misa);
    if (misa == 0) {
        printf("misa is empty\n");
    } else {
        // Print extensions
        printf("Supported extensions: ");
        for (int i = 0; i <= 25; i++) {
            if ((misa & (1<<i)) != 0) {
                printf("%c", 'A' + i);
            }
        }
        printf("\n");
    }

    print_fdt_info(dtb);

    size_t mideleg = 0;
    csr_write("mideleg", mideleg);
    csr_read("mideleg", mideleg);
    printf("mideleg = %#zx\n", mideleg);

    printf("printf test: %s\n", "eqwe");

    init_traps();
    enable_interrupts();

    while(1) {
        printf("counter: %d\r", counter);
        __asm__("wfi");
    }
}
