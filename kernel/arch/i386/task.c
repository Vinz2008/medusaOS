#include <kernel/task.h>
#include <stdio.h>

static Task *runningTask;
static Task mainTask;
static Task otherTask;

static void otherMain() {
    log(LOG_SERIAL, true, "Hello multitasking world!\n"); // Not implemented here...
    yield();
}

void initTasking() {
    // Get EFLAGS and CR3
    asm volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(mainTask.regs.cr3)::"%eax");
    asm volatile("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(mainTask.regs.eflags)::"%eax");

    createTask(&otherTask, otherMain, mainTask.regs.eflags, (uint32_t*)mainTask.regs.cr3);
    mainTask.next = &otherTask;
    otherTask.next = &mainTask;

    runningTask = &mainTask;
}

void createTask(Task *task, void (*main)(), uint32_t flags, uint32_t *pagedir) {
    task->regs.eax = 0;
    task->regs.ebx = 0;
    task->regs.ecx = 0;
    task->regs.edx = 0;
    task->regs.esi = 0;
    task->regs.edi = 0;
    task->regs.eflags = flags;
    task->regs.eip = (uint32_t) main;
    task->regs.cr3 = (uint32_t) pagedir;
    //task->regs.esp = (uint32_t) allocPage() + 0x1000; // Not implemented here
    task->regs.esp = (uint32_t) kmalloc(4000) + 0x1000; // Not implemented here
    task->next = 0;
}

void yield() {
    Task *last = runningTask;
    runningTask = runningTask->next;
    switchTask(&last->regs, &runningTask->regs);
}

void test_switch_task(){
    log(LOG_SERIAL, false, "Switching to otherTask... \n");
    yield();
    log(LOG_SERIAL, false, "Returned to mainTask!\n");
}