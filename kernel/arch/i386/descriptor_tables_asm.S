.global flush_gdt
flush_gdt:
    mov %eax, 4(%esp) 
    lgdt (%eax)
    mov %ax, 0x10
    mov %ds, %ax
    mov %es, %ax
    mov %fs, %ax
    mov %gs, %ax
    mov %ss, %ax
    push 0x08
    push flush
    retf
    
flush:
    ret

.global flush_idt

flush_idt:
    mov %eax, 4(%esp)
    lidt (%eax)
    ret

.global flush_tss
flush_tss:
    mov %ax, 0x2B 
    ltr %ax
    ret