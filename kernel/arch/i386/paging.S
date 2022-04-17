.global start_paging

start_paging:
    mov %cr3, %eax

    mov %ebx, %cr4
    or  %ebx, 0x00000010
    mov %cr4, %ebx

    mov %ebx, %cr0
    or  %ebx, 0x80000000
    mov %cr0, %ebx
    retf