.global syscall
syscall:
    mov 4(%esp), %eax
    int $0x80
    ret

.global syscall1
syscall1: # eax, ebx
    push %ebx
    mov 8(%esp), %eax
    mov 12(%esp), %ebx
    int $0x80
    pop %ebx
    ret

.global syscall2
syscall2: # eax, ebx, ecx
    push %ebx
    push %ecx
    mov 12(%esp), %eax
    mov 16(%esp), %ebx
    mov 20(%esp), %ecx
    int $0x80
    pop %ecx
    pop %ebx
    ret

.global syscall3
syscall3: # eax, ebx, ecx, edx
    push %ebx
    push %ecx
    push %edx
    mov 16(%esp), %eax
    mov 20(%esp), %ebx
    mov 24(%esp), %ecx
    mov 28(%esp), %edx
    int $0x80
    pop %edx
    pop %ecx
    pop %ebx
    ret