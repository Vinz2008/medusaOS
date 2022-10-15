#include <kernel/syscall.h>
#include <kernel/irq_handlers.h>
#include <kernel/vfs.h>


void syscall_handler(x86_iframe_t * frame){
    switch (frame->ax){
    case SYS_READ:
        break;
    case SYS_WRITE:
        vfs_write_fd(frame->di, frame->si, frame->dx);
        break;
    
    default:
        break;
    }
}

void init_syscalls(){
    irq_register_handler(0x80, syscall_handler);
}