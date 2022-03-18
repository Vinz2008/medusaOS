.global gdt_flush

gdt_flush:
    mov %eax, 4(%esp)
    lgdt (%eax)
    push 0x80
    push flush
    retf
flush:
    ret


.extern isr_handler
isr_common_stub:
   pusha
   mov %ax, %ds
   push %eax 

   call isr_handler
   popa
   add %esp, 8  
   iret    


.global load_idt

load_idt:
  mov   eax, 4(esp)
  lidt eax
  ret


.macro no_error_code_interrupt_handler nb
.global interrupt_handler_nb
interrupt_handler_nb
  push 0
  push nb
  jmp common_interrupt_handler
.endm

.macro error_code_interrupt_handler nb
.global interrupt_handler_nb
interrupt_handler_nb
  push nb
  jmp common_interrupt_handler
.endm




no_error_code_interrupt_handler 0
no_error_code_interrupt_handler 1 
no_error_code_interrupt_handler 2
no_error_code_interrupt_handler 3 
no_error_code_interrupt_handler 4 
no_error_code_interrupt_handler 5 
no_error_code_interrupt_handler 6 
error_code_handler 7
error_code_handler 8
error_code_handler 9
error_code_handler 10
error_code_handler 11
error_code_handler 12
error_code_handler 13
error_code_handler 14
error_code_handler 15
error_code_handler 16
error_code_handler 17
error_code_handler 18
error_code_handler 19
error_code_handler 20
error_code_handler 21
error_code_handler 22
error_code_handler 23
error_code_handler 24
error_code_handler 25
error_code_handler 26
error_code_handler 27
error_code_handler 28
error_code_handler 29
error_code_handler 30
error_code_handler 

