#include <stdint.h>
#include <printf.h>
#include <csr.h>

extern long long add(long long a, long long b);

int kernel_main() {
    long long i = add(4, 5);
    printf("%d", i);
    printf("Hello, rv64!\n");

    size_t misa;
    csr_read("misa", misa);
    if (misa == 0) {
        printf("misa is empty\n");
    } else {
        size_t mxlen = get_mxlen(misa);
        printf("mxlen = %d\n", mxlen);

        // Print extensions
        printf("Supported extensions: ");
        for (int i = 0; i <= 25; i++) {
            if ((misa & (1<<i)) != 0) {
                printf("%c", 'A' + i);
            }
        }
        printf("\n");
    }
    size_t mvendorid;
    csr_read("mvendorid", mvendorid);

    printf("mvendorid = 0x%zX\n", mvendorid);

    printf("printf test: %s\n", "eqwe");

    size_t mstatus;
    csr_read("mstatus", mstatus);
    mstatus |= 8;
    csr_write("mstatus", mstatus);

    csr_read("mstatus", mstatus);
    printf("mstatus: %d\n", mstatus);
    while(1) {
        __asm__("wfi");
    }
}
