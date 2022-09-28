#include <kernel/x86.h>

#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_STAT 4


void syscall_handler(x86_iframe_t * frame);

void init_syscalls();
