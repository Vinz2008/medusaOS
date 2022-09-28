#include <kernel/syscall.h>
#include <kernel/irq_handlers.h>
#include <kernel/vfs.h>


void syscall_handler(x86_iframe_t * frame){
    switch (frame->ax){
    case SYS_READ:
        vfs_write(frame->di, frame->si, frame->dx);
        break;
    case SYS_WRITE:
        break;
    
    default:
        break;
    }
}

void init_syscalls(){
    irq_register_handler(0x80, syscall_handler);
}