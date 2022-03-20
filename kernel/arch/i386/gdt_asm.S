.global gdt_flush

gdt_flush:
    mov %eax, 4(%esp)
    lgdt (%eax)
    mov %ds, 0x10
    mov %ss, 0x10
    mov %es, 0x10
    push 0x08
    push flush_cs
    retf
flush_cs:
    ret

