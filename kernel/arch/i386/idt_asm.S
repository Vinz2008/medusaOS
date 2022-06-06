.macro no_error_code_interrupt_handler input="1"
.global interrupt_handler_\input
    interrupt_handler_\input:
        push    0 
        push    \input
        jmp     exception_handler
        iret
.endm

.macro error_code_interrupt_handler input="1"
.global interrupt_handler_\input
interrupt_handler_\input:
    push    \input
    jmp     exception_handler
    iret
.endm

.extern exception_handler

no_error_code_interrupt_handler input="0"
no_error_code_interrupt_handler input="1"
no_error_code_interrupt_handler input="2"
no_error_code_interrupt_handler input="3"
no_error_code_interrupt_handler input="4"
no_error_code_interrupt_handler input="5"
no_error_code_interrupt_handler input="6"
no_error_code_interrupt_handler input="7"
error_code_interrupt_handler input="8"  
no_error_code_interrupt_handler input="9" 
error_code_interrupt_handler input="10"  
error_code_interrupt_handler input="11" 
error_code_interrupt_handler input="12" 
error_code_interrupt_handler input="13"
error_code_interrupt_handler input="14" 
no_error_code_interrupt_handler input="15" 
no_error_code_interrupt_handler input="16"  
error_code_interrupt_handler input="17" 
no_error_code_interrupt_handler input="18" 
no_error_code_interrupt_handler input="19"
no_error_code_interrupt_handler input="20" 
no_error_code_interrupt_handler input="21" 
no_error_code_interrupt_handler input="22" 
no_error_code_interrupt_handler input="23" 
no_error_code_interrupt_handler input="24" 
no_error_code_interrupt_handler input="25" 
no_error_code_interrupt_handler input="26" 
no_error_code_interrupt_handler input="27" 
no_error_code_interrupt_handler input="28" 
no_error_code_interrupt_handler input="29" 
error_code_interrupt_handler input="30"
no_error_code_interrupt_handler input="31"




.global load_idt

load_idt:
    mov %eax, 4(%esp)
    lidt (%eax)
    ret
