#ifndef __TASK_H__
#define __TASK_H__

#include <kernel/x86.h>
#include <stdint.h>

extern void initTasking();

typedef struct {
  uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} Registers_task;

typedef struct Task {
  Registers_task regs;
  struct Task* next;
} Task;

extern void initTasking();
extern void createTask(Task*, void (*)(), uint32_t, uint32_t*);

extern void yield(); // Switch task frontend
extern void
switchTask(Registers_task* old,
           Registers_task* new); // The function which actually switches
void test_switch_task();

#endif /* __TASK_H__ */