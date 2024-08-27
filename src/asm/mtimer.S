.global write_mtime
.global read_mtime
.global write_mtimecmp
.global read_mtimecmp
.equ mtimecmp, 0x2004000
.equ mtime, 0x200BFF8

write_mtime:
    li t0, mtime
    sd a0, 0(t0)
    ret

read_mtime:
    li t0, mtime
    ld a0, 0(t0)
    ret

write_mtimecmp:
    li t0, mtimecmp
    sd a0, 0(t0)
    ret

read_mtimecmp:
    li t0, mtimecmp
    ld a0, 0(t0)
    ret
