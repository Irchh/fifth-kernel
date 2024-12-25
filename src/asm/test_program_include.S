.global test_program_start
.global test_program_end
.section .data.raw

test_program_start:
.incbin "../test_program/test_program"
test_program_end: