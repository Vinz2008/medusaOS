
.global _start

.section .text

_start:
    movl $1, %eax
    movl $1, %edi
    movl $msg, %esi
    movl $13, %edx
    int $0x80

msg:
    .ascii "Hello from module\n"