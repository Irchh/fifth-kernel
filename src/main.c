#include <stdint.h>
#include <printf.h>
#include <csr.h>
#include "trap.h"
#include "fdt.h"
#include "frame.h"
#include "priv.h"
#include "mem.h"

extern long long add(long long a, long long b);
extern unsigned long long counter;

int _kernel_main(struct fdt_header* dtb) {
    long long i = add(4, 5);
    printf("%d", i);
    printf("Hello, rv64!\n");

    print_fdt_info(dtb);

    init_frames();

    size_t frame1 = device_information.ram_start + 4096*first_free_frame();
    allocate_frame(frame1);
    printf("Frame1: %#zx\n", frame1);
    memset((char*)(frame1), 0, 4096);

    // TODO: blablablabla page table stuff and memory

    init_traps();
    enable_interrupts();

    printf("RAM START: %#zx\n", device_information.ram_start);
    printf("RAM SIZE:  %#zx (%zu MiB)\n", device_information.ram_size, device_information.ram_size/1024/1024);

    uint64_t counter2 = 0;

    while(1) {
        printf("counter: %d (%zd)\r", counter, counter2++);
        __asm__("wfi");
        __asm__("ecall");
    }
}

int kernel_main(uint64_t _idk, struct fdt_header* dtb) {
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

    init_traps();
    enable_interrupts();

    set_priv_m(01, &_kernel_main, dtb);
}
