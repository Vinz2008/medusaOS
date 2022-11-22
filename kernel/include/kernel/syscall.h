#include <kernel/x86.h>

#define SYSCALL_INTERRUPT "$0x80" // for inline assembly

#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_STAT 4
#define SYS_SBRK 5
#define SYS_MAX 6 // First invalid syscall number

#define SYSCALL_NUM 64

void syscall_handler(x86_iframe_t * frame);
typedef void (*handler_t)(x86_iframe_t*);

void init_syscalls();
