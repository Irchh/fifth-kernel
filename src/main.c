#include <stdint.h>
#include <printf.h>
#include <csr.h>
#include "trap.h"

extern long long add(long long a, long long b);
extern unsigned long long counter;

int kernel_main() {
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
