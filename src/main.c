#include <stdint.h>
#include <printf.h>
#include <csr.h>
#include "trap.h"
#include "fdt.h"
#include "frame.h"
#include "priv.h"
#include "page.h"

extern long long add(long long a, long long b);
extern unsigned long long counter;

void kernel_main() {
    uint64_t counter2 = 0;

    printf("Addr of counter: %#zx\n", &counter);
    printf("Addr of counter2: %#zx\n", &counter2);

    while(1) {
        printf("counter: %d (%zd)\r", counter, counter2++);
        __asm__("wfi");
        __asm__("ecall");
    }
}

void kernel_init(struct fdt_header* dtb) {
    long long i = add(4, 5);
    printf("%d", i);
    printf("Hello, rv64!\n");

    print_fdt_info(dtb);

    init_frames();

    enable_paging();

    init_traps();
    enable_interrupts();

    printf("RAM START: %#zx\n", device_information.ram_start);
    printf("RAM SIZE:  %#zx (%zu MiB)\n", device_information.ram_size, device_information.ram_size/1024/1024);

    // Modify the stack to use the higher address
    __asm__(
        "add sp, sp, %0\n"
        : : "r" (HIGHER_HALF_ADDR_START - kernel_start_addr)
    );
    void (*kernel_main_hi)() = &kernel_main + HIGHER_HALF_ADDR_START - kernel_start_addr;
    kernel_main_hi();
    //kernel_main();
}

int kernel_start(uint64_t _idk, struct fdt_header* dtb) {
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

    // Delegate traps to supervisor mode
    size_t mideleg = 0xFFFF;
    size_t medeleg = 0xFFFF;
    csr_write("mideleg", mideleg);
    csr_write("medeleg", medeleg);

    set_priv_m(01, &kernel_init, dtb);
    return -1; // init failed ig
}
