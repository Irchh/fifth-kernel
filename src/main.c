#include <stdint.h>
#include <printf.h>
#include <csr.h>
#include "trap.h"
#include "fdt.h"
#include "frame.h"
#include "priv.h"

extern long long add(long long a, long long b);
extern unsigned long long counter;

void supervisor_test() {
    printf("Hi from supervisor!\n");
    while (1);
}

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

    init_frames();

    size_t frame1 = first_free_frame();
    allocate_frame(frame1);
    printf("Frame1: %#zx\n", frame1);

    size_t frame2 = first_free_frame();
    allocate_frame(frame2);
    printf("Frame2: %#zx\n", frame2);

    printf("Deallocating Frame1\n");
    deallocate_frame(frame1);
    size_t frame3 = first_free_frame();
    allocate_frame(frame3);
    printf("Frame3: %#zx\n", frame3);


    size_t mconfigptr = 0;
    csr_read("mconfigptr", mconfigptr);
    printf("mconfigptr: %#zx\n", mconfigptr);

    init_traps();
    enable_interrupts();

    set_priv_m(01, &supervisor_test);

    printf("RAM START: %#zx\n", device_information.ram_start);
    printf("RAM SIZE:  %#zx (%d MiB)\n", device_information.ram_size, device_information.ram_size/1024/1024);

    while(1) {
        printf("counter: %d\r", counter);
        __asm__("wfi");
        __asm__("ecall");
    }
}
