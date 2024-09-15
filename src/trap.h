#include <stddef.h>

#define TRAP_ECALL 0xB

#define INT_TIMER 0x7

typedef struct {
    size_t mepc;
    size_t x1;
    size_t x2;
    size_t x3;
    size_t x4;
    size_t x5;
    size_t x6;
    size_t x7;
    size_t x8;
    size_t x9;
    size_t x10;
    size_t x11;
    size_t x12;
    size_t x13;
    size_t x14;
    size_t x15;
    size_t x16;
    size_t x17;
    size_t x18;
    size_t x19;
    size_t x20;
    size_t x21;
    size_t x22;
    size_t x23;
    size_t x24;
    size_t x25;
    size_t x26;
    size_t x27;
    size_t x28;
    size_t x29;
    size_t x30;
    size_t x31;
} register_state_t;

void init_traps();
void enable_interrupts();
