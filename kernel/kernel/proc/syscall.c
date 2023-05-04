#include <kernel/syscall.h>
#include <stdio.h>
#include <kernel/irq_handlers.h>
#include <kernel/vfs.h>


handler_t syscall_handlers[SYSCALL_NUM] = { 0 };


void syscall_handler(registers_t* frame){
    log(LOG_SERIAL, false, "test syscall handler\n");
    if(frame->eax < SYS_MAX && syscall_handlers[frame->eax]){
        handler_t handler = syscall_handlers[frame->eax];
        frame->eax = 0;
        handler(frame);
    } else {
        log(LOG_SERIAL, false, "unknown syscall : %d\n", frame->eax);
    }
}

void syscall_read(registers_t* frame){
    log(LOG_SERIAL, false, "syscall read test\n");
}

void syscall_write(registers_t* frame){
    uint32_t fd = frame->ebx;
    uint8_t* buf = (uint8_t*)frame->ecx;
    //log(LOG_SERIAL, false, "buf : %c\n", *buf);
    uint32_t size = frame->edx;
    frame->eax = vfs_write_fd(fd, buf, size);
}

void syscall_sbrk(registers_t* frame){
    
}

void init_syscalls(){
    irq_register_handler(0x80, syscall_handler);
    syscall_handlers[SYS_READ] = syscall_read;
    syscall_handlers[SYS_WRITE] = syscall_write;
    syscall_handlers[SYS_SBRK] = syscall_sbrk;
}
