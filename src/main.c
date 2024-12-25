#include <stdint.h>
#include <printf.h>
#include <csr.h>
#include "trap.h"
#include "fdt.h"
#include "frame.h"
#include "priv.h"
#include "page.h"

extern long long add(long long a, long long b);
extern void setup_post_relocation(size_t relocation_addr_diff);
extern unsigned long long counter;

extern uint64_t rodata_start;
extern uint64_t rodata_middle;
extern uint64_t rodata_end;

// Probably an insane function, but it is needed for relocating since this section contains a bunch of pointers that need to be modified
// Gets called from asm (post_relocation.S)
// TODO: Verify accuracy of while statement
void modify_rodata_ptrs() {
    uint64_t* ptr_ptr = &rodata_start;
    while (*ptr_ptr >= kernel_start_addr && *ptr_ptr < device_information.ram_start + device_information.ram_size && ptr_ptr != &rodata_middle) {
        *ptr_ptr = *ptr_ptr + HIGHER_HALF_ADDR_START - kernel_start_addr;
        *ptr_ptr++;
    }
}

extern uint64_t test_program_start;
extern uint64_t test_program_end;

void kernel_main() {
    counter = 69;
    uint64_t counter2 = 0;

    unmap_lower_half_kernel();

    printf("test_program_start: %#zx\n", &test_program_start);
    printf("test_program_end: %#zx\n", &test_program_end);

    while(1) {
        printf("counter: %zd (%zd)\r", counter, counter2++);
        __asm__("wfi");
        __asm__("ecall");
    }
}

void kernel_init(struct fdt_header* dtb) {
    long long i = add(4, 5);
    printf("%zd", i);
    printf("Hello, rv64!\n");

    print_fdt_info(dtb);

    init_frames();

    enable_paging();

    init_traps();
    enable_interrupts();

    printf("RAM START: %#zx\n", device_information.ram_start);
    printf("RAM SIZE:  %#zx (%zu MiB)\n", device_information.ram_size, device_information.ram_size/1024/1024);

    // Modify the stack to use the higher address
    void (*kernel_main_hi)(size_t) = &setup_post_relocation + HIGHER_HALF_ADDR_START - kernel_start_addr;
    kernel_main_hi(HIGHER_HALF_ADDR_START - kernel_start_addr);
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
            if ((misa & 1<<i) != 0) {
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

    set_priv_m(01, (void*)&kernel_init, dtb);
    return -1; // init failed ig
}
