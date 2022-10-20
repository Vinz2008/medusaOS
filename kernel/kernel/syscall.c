#include <kernel/syscall.h>
#include <kernel/irq_handlers.h>
#include <kernel/vfs.h>


void syscall_handler(x86_iframe_t * frame){
    switch (frame->eax){
    case SYS_READ:
        break;
    case SYS_WRITE:
        vfs_write_fd(frame->edi, frame->esi, frame->edx);
        break;
    
    default:
        break;
    }
}

void init_syscalls(){
    irq_register_handler(0x80, syscall_handler);
}