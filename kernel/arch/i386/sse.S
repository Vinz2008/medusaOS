.global sse_available

sse_available:
    movl $0x1, %eax
    cpuid
    test %edx, 1 << 25
    movl $0x1, %eax
    jnz good
    mov $0, %eax

good:
    ret

.global sse_init

sse_init:
    mov %cr0, %eax
    and $0xFFFB, %ax
    or $0x02, %ax
    mov %eax, %cr0
    mov %cr4, %eax
    or %ax, 3 << 9
    mov %eax, %cr4
    ret