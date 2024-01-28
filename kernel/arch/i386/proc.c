#include <kernel/kmalloc.h>
#include <kernel/misc.h>
#include <kernel/paging.h>
#include <kernel/pmm.h>
#include <kernel/proc.h>
#include <kernel/scheduler.h>
#include <kernel/x86.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <types.h>

extern uint32_t irq_handler_end;
process_t* current_process = NULL;
scheduler_t* scheduler = NULL;
static uint32_t next_pid = 1;

void init_proc() {
  // scheduler = scheduler_medusaos();
}

process_t* proc_run_code(uint8_t* code, uint32_t size, char** argv, int argc) {
  static uintptr_t temp_page = 0;

  if (!temp_page) {
    temp_page = (uintptr_t)kamalloc(0x1000, 0x1000);
  }
  char** argv_copy = kmalloc(sizeof(char*) * argc);
  for (int i = 0; i < argc; i++) {
    argv_copy[i] = strdup(argv[i]);
  }
  uint32_t num_code_pages = divide_up(size, 0x1000);
  uint32_t num_stack_pages = PROC_STACK_PAGES;
  process_t* process = kmalloc(sizeof(process_t));
  uintptr_t kernel_stack =
      (uintptr_t)aligned_alloc(4, 0x1000 * PROC_KERNEL_STACK_PAGES);
  uintptr_t pd_phys = pmm_alloc_page();
  // Copy the kernel page directory with a temporary mapping
  page_t* p = paging_get_page(temp_page, false, 0);
  *p = pd_phys | PAGE_PRESENT | PAGE_RW;
  memcpy((void*)temp_page, (void*)0xFFFFF000, 0x1000);
  directory_entry_t* pd = (directory_entry_t*)temp_page;
  pd[1023] = pd_phys | PAGE_PRESENT | PAGE_RW;
  // ">> 22" grabs the address's index in the page directory, see `paging.c`
  for (uint32_t i = 0; i < (KERNEL_BASE_VIRT >> 22); i++) {
    pd[i] = 0; // Unmap everything below the kernel
  }
  uintptr_t previous_pd = *paging_get_page(0xFFFFF000, false, 0) & PAGE_FRAME;
  paging_switch_directory(pd_phys);
  // Map the code and copy it to physical pages, zero out the excess memory
  // for static variables
  // TODO: don't require contiguous pages
  uintptr_t code_phys = pmm_alloc_pages(num_code_pages);
  paging_map_pages(0x00001000, code_phys, num_code_pages, PAGE_USER | PAGE_RW);
  memcpy((void*)0x00001000, (void*)code, size);
  memset((uint8_t*)0x1000 + size, 0, num_code_pages * 0x1000 - size);
  uintptr_t stack_phys = pmm_alloc_pages(num_stack_pages);
  paging_map_pages(0xC0000000 - 0x1000 * num_stack_pages, stack_phys,
                   num_stack_pages, PAGE_USER | PAGE_RW);
  char* ustack_char = (char*)(0xC0000000 - 1);
  char* arg;
  struct args_proc arg_list;
  arg_list.argc = argc;
  arg_list.argv = kmalloc(sizeof(char*) * argc);
  for (int i = 0; i < argc; i++) {
    arg = argv_copy[i];
    uint32_t len = strlen(arg);
    // We need (ustack_char - len) to be 4-bytes aligned
    ustack_char -= ((uintptr_t)ustack_char - len) % 4;
    char* dest = ustack_char - len;

    strncpy(dest, arg, len);
    ustack_char -= len + 1; // Keep pointing to a free byte
    arg_list.argv[i] = dest;
    kfree(arg);
  }
  uint32_t* ustack_int = (uint32_t*)((uintptr_t)ustack_char & ~0x3);
  uint32_t arg_count = 0;
  for (int i = 0; i < argc; i++) {
    arg = arg_list.argv[i];
    *(ustack_int--) = (uintptr_t)arg;
    arg_count++;
  }
  // Push program arguments
  uintptr_t argsptr = (uintptr_t)(ustack_int + 1);
  *(ustack_int--) = arg_count ? argsptr : (uintptr_t)NULL;
  *(ustack_int--) = arg_count;
  // Switch to the original page directory
  paging_switch_directory(previous_pd);
  *process = (process_t){
      .directory = pd_phys,
  };
  /* *process = (process_t){
      .pid = next_pid++,
      .code_len = num_code_pages,
      .stack_len = num_stack_pages,
      .directory = pd_phys,
      .kernel_stack = kernel_stack + PROC_KERNEL_STACK_PAGES * 0x1000 - 4,
      .saved_kernel_stack = kernel_stack + PROC_KERNEL_STACK_PAGES * 0x1000 - 4,
      .initial_user_stack = (uintptr_t) ustack_int,
      .mem_len = 0,
      .sleep_ticks = 0,
      .cwd = strdup("/")
  };*/
  uint32_t* jmp = &irq_handler_end;
  // Setup the process's kernel stack as if it had already been interrupted
  /*asm volatile (
      // Save our stack in %ebx
      "mov %%esp, %%ebx\n"

      // Temporarily use the new process's kernel stack
      "mov %[kstack], %%eax\n"
      "mov %%eax, %%esp\n"

      // Stuff popped by `iret`
      "push $0x23\n"         // user ds selector
      "mov %[ustack], %%eax\n"
      "push %%eax\n"         // %esp
      "push $0x202\n"        // %eflags with `IF` bit set
      "push $0x1B\n"         // user cs selector
      "push $0x00001000\n"   // %eip
      // Push error code, interrupt number
      "sub $8, %%esp\n"
      // `pusha` equivalent
      "sub $32, %%esp\n"
      // push data segment registers
      "mov $0x20, %%eax\n"
      "push %%eax\n"
      "push %%eax\n"
      "push %%eax\n"
      "push %%eax\n"

      // Push proc_switch_process's `ret` %eip
      "mov %[jmp], %%eax\n"
      "push %%eax\n"
      // Push garbage %ebx, %esi, %edi, %ebp
      "push $1\n"
      "push $2\n"
      "push $3\n"
      "push $4\n"

      // Save the new process's %esp in %eax
      "mov %%esp, %%eax\n"
      // Restore our stack
      "mov %%ebx, %%esp\n"
      // Update the new process's %esp
      "mov %%eax, %[esp]\n"
      : [esp] "=r" (process->saved_kernel_stack)
      : [kstack] "r" (process->kernel_stack),
        [ustack] "r" (process->initial_user_stack),
        [jmp] "r" (jmp)
      : "%eax", "%ebx"
  );*/
  scheduler->sched_add(scheduler, process);

  return process;
}

void proc_enter_usermode() {
  x86_disable_int(); // Interrupts will be reenabled by `iret`

  current_process = scheduler->sched_get_current(scheduler);

  if (!current_process) {
    printf("no process to run\n");
    abort();
  }
  paging_switch_directory(current_process->directory);

  /*asm volatile (
      "mov $0x23, %%eax\n"
      "mov %%eax, %%ds\n"
      "mov %%eax, %%es\n"
      "mov %%eax, %%fs\n"
      "mov %%eax, %%gs\n"
      "push %%eax\n"       // %ss
      "mov %[ustack], %%eax\n"
      "push %%eax\n"       // %esp
      "push $0x202\n"      // %eflags with IF set
      "push $0x1B\n"       // %cs
      "push $0x00001000\n" // %eip
      "iret\n"
      :: [ustack] "r" (current_process->initial_user_stack)
      : "%eax");*/
}