#include <stdint.h>
#include <printf.h>

#define csr_read(csr, dst) __asm__ (\
    "csrr %0, "csr\
    : "=r" (dst)\
)

extern long long add(long long a, long long b);

size_t get_mxlen(size_t misa) {
    return misa>>(sizeof(size_t)*8-2);
}
 
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

    printf("printf test: %s\n", "eqwe");

    while(1);
}
