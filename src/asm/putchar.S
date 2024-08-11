.global putchar
.global _putchar

putchar:
    lui a1, 0x10000
    sb a0, 0(a1)
    ret

_putchar:
    j putchar
