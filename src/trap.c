#include <csr.h>
#include <printf.h>
#include <stdint.h>
#include "trap.h"

#include "frame.h"
#include "page.h"

static unsigned char TRAP_STACK[4096] __attribute__ ((aligned (4096)));
unsigned long long counter __attribute__((aligned(16))) = 0;

__attribute__((naked, aligned)) void naked_trap_handler(void) {
    __asm__(
        "csrrw sp, sscratch, sp\n"
        "addi sp, sp, -256\n"

        "sd x1, (1*8)(sp)\n"
        "sd x2, (2*8)(sp)\n"
        "sd x3, (3*8)(sp)\n"
        "sd x4, (4*8)(sp)\n"
        "sd x5, (5*8)(sp)\n"
        "sd x6, (6*8)(sp)\n"
        "sd x7, (7*8)(sp)\n"
        "sd x8, (8*8)(sp)\n"
        "sd x9, (9*8)(sp)\n"
        "sd x10, (10*8)(sp)\n"
        "sd x11, (11*8)(sp)\n"
        "sd x12, (12*8)(sp)\n"
        "sd x13, (13*8)(sp)\n"
        "sd x14, (14*8)(sp)\n"
        "sd x15, (15*8)(sp)\n"
        "sd x16, (16*8)(sp)\n"
        "sd x17, (17*8)(sp)\n"
        "sd x18, (18*8)(sp)\n"
        "sd x19, (19*8)(sp)\n"
        "sd x20, (20*8)(sp)\n"
        "sd x21, (21*8)(sp)\n"
        "sd x22, (22*8)(sp)\n"
        "sd x23, (23*8)(sp)\n"
        "sd x24, (24*8)(sp)\n"
        "sd x25, (25*8)(sp)\n"
        "sd x26, (26*8)(sp)\n"
        "sd x27, (27*8)(sp)\n"
        "sd x28, (28*8)(sp)\n"
        "sd x29, (29*8)(sp)\n"
        "sd x30, (30*8)(sp)\n"
        "sd x31, (31*8)(sp)\n"

        "csrr x1, sepc\n"
        "sd x1, (0*8)(sp)\n"

        "addi a0, sp, 0\n"

        "call trap_handler\n"

        "ld x1, (0*1)(sp)\n"
        "csrw sepc, x1\n"

        "ld x1, (1*8)(sp)\n"
        // Don't restore sp here
        "ld x3, (3*8)(sp)\n"
        "ld x4, (4*8)(sp)\n"
        "ld x5, (5*8)(sp)\n"
        "ld x6, (6*8)(sp)\n"
        "ld x7, (7*8)(sp)\n"
        "ld x8, (8*8)(sp)\n"
        "ld x9, (9*8)(sp)\n"
        "ld x10, (10*8)(sp)\n"
        "ld x11, (11*8)(sp)\n"
        "ld x12, (12*8)(sp)\n"
        "ld x13, (13*8)(sp)\n"
        "ld x14, (14*8)(sp)\n"
        "ld x15, (15*8)(sp)\n"
        "ld x16, (16*8)(sp)\n"
        "ld x17, (17*8)(sp)\n"
        "ld x18, (18*8)(sp)\n"
        "ld x19, (19*8)(sp)\n"
        "ld x20, (20*8)(sp)\n"
        "ld x21, (21*8)(sp)\n"
        "ld x22, (22*8)(sp)\n"
        "ld x23, (23*8)(sp)\n"
        "ld x24, (24*8)(sp)\n"
        "ld x25, (25*8)(sp)\n"
        "ld x26, (26*8)(sp)\n"
        "ld x27, (27*8)(sp)\n"
        "ld x28, (28*8)(sp)\n"
        "ld x29, (29*8)(sp)\n"
        "ld x30, (30*8)(sp)\n"
        "ld x31, (31*8)(sp)\n"

        "addi sp, sp, 256\n"
        "csrrw sp, sscratch, sp\n" // Should we swap sp and sscratch here, or load the trap handler sp some other way?
        "sret"
    );
}

extern uint64_t read_mtime(void);
extern void write_mtimecmp(uint64_t value);

void handle_interrupt(long long cause, register_state_t* regs) {
    //printf("Interrupt! cause: %#zX\n", cause);
    // Timer
    if (cause == INT_M_TIMER || cause == INT_S_TIMER) {
        counter++;
        write_mtimecmp(read_mtime() + 1000000);
    } else {
        printf("Interrupt! cause: %#zX\n", cause);
    }
}

void handle_trap(long long cause, register_state_t* regs) {
    switch (cause) {
        case TRAP_M_ECALL: {
            regs->mepc += 4;
            break;
        }
        case TRAP_S_ECALL: {
            regs->mepc += 4;
            break;
        }
        default: {
            printf("Trap! cause: %#zX\n", cause);
            while(1);
        }
    }
}

void trap_handler(register_state_t* regs) {
    size_t scause;
    long long cause;
    csr_read("scause", scause);
    cause = scause & (~(1L<<63));
    if ((long long)scause < 0) {
        handle_interrupt(cause, regs);
    } else {
        handle_trap(cause, regs);
    }
}

void init_traps() {
    printf("Init stvec to trap handler function\n");
    csr_write("stvec", ((size_t)&naked_trap_handler + HIGHER_HALF_ADDR_START - kernel_start_addr)&~0b11);
    csr_write("sscratch", &TRAP_STACK[4096]);
}

void enable_interrupts() {
    printf("Enabling Interrupt\n");

    size_t sstatus;
    csr_read("sstatus", sstatus);
    sstatus |= 0x2; // Enable interrupt
    csr_write("sstatus", sstatus);
    csr_write("sie", 0xFF);
}
