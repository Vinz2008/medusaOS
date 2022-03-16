.global load_lgdt
load_lgdt:
    lgdt (%eax)
    mov %ds, 0x10
    mov %ss, 0x10
    mov %es, 0x10
    jmp 0x08