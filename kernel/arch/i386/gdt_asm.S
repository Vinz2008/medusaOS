.global gdt_flush

.extern gp
gdt_flush:
    lgdt (gp)
    mov %ax, 0x10
	mov %ds, %ax
	mov %es, %ax
	mov %fs, %ax
	mov %gs, %ax
	mov %ss, %ax
    push 0x08
    push flush_cs
    retf
flush_cs:
    ret


.global tss_flush
tss_flush:
	mov %ax, 0x2B
	ltr %ax
	ret
