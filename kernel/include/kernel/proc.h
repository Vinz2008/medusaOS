#include <types.h>
#include <sys/types.h>
#include <kernel/vfs.h>

#ifndef _PROC_MEDUSAOS_HEADER_
#define _PROC_MEDUSAOS_HEADER_

#define PROC_STACK_PAGES 4
#define PROC_KERNEL_STACK_PAGES 1
#define PROC_MAX_FD 1024

struct args_proc {
    int argc;
    char** argv;
};

typedef struct _proc_t {
    pid_t pid;
    char* name; 
    char*        description;
    // Sizes of the exectuable and of the stack in number of pages
    uint32_t stack_len;
    uint32_t code_len;
    uintptr_t directory;
    // Kernel stack used when the process is preempted
    uintptr_t kernel_stack;
    // Kernel stack to restore when the process is switched to
    uintptr_t saved_kernel_stack;
    // Stack to use when first switching to userspace for a new process
    uintptr_t initial_user_stack;
    uint32_t mem_len; // Size of program heap in bytes
    uint32_t sleep_ticks;
    uint8_t fpu_registers[512];
    //list_t filetable;
    char* cwd;
    fs_node_t* cwd_node;
} process_t;

typedef struct _sched_t {
    /* Returns the currently elected process */
    process_t* (*sched_get_current)(struct _sched_t*);
    /* Adds a new process, already initialized, to the process pool */
    void (*sched_add)(struct _sched_t*, process_t*);
    /* Returns the next process that should be run, depending to the specific
       scheduler implemented. Note that it can choose not to change process by
       returning the currently executing process */
    process_t* (*sched_next)(struct _sched_t*);
    /* Removes a process from the process pool. Basically the inverse of
     * `sched_add`. If the removed process was the one currently executing, the
     * scheduler must ensure that `sched_next` keeps working: it'll be called
     * right after.
     */
    void (*sched_exit)(struct _sched_t*, process_t*);
} scheduler_t;

#endif