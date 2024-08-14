#include <csr.h>
#include <printf.h>

__attribute__((naked, aligned)) void naked_trap_handler(void) {
    __asm__(
        "jal trap_handler\n"
        "mret"
    );
}

void handle_interrupt(long long cause) {
    printf("Interrupt! cause: %#zX\n", cause);
}

void handle_trap(long long cause) {
    printf("Trap! cause: %#zX\n", cause);
}

void trap_handler(void) {
    size_t mcause;
    long long cause;
    csr_read("mcause", mcause);
    cause = mcause & (~(1L<<63)); 
    if ((long long)mcause < 0) {
        handle_interrupt(cause);
    } else {
        handle_trap(cause);
    }
    while(1);
}

void init_traps() {
    printf("Init mtvec to trap handler function\n");
    csr_write("mtvec", ((size_t)&naked_trap_handler)&~0b11);
}

void enable_interrupts() {
    printf("Enabling Interrupt\n");

    size_t mstatus;
    csr_read("mstatus", mstatus);
    mstatus |= 0x8; // Enable interrupt
    csr_write("mstatus", mstatus);
    csr_write("mie", 0xFF);
}
