#include <kernel/fpu.h>
#include <stddef.h>
#include <types.h>

#define CR4_OSFXSR (1 << 9)
#define CR4_OSXMMEXCPT (1 << 10)
#define CR0_MP (1 << 1)
#define CR0_EM (1 << 2)

static uint8_t kernel_fpu[512] __attribute__((aligned(16)));

void fpu_init() {
  uint32_t cr;
  /* Configure CR0: disable emulation (EM), as we assume we have an FPU, and
   * enable the EM bit: with the TS and EM bits disabled, `wait/fwait`
   * instructions won't generate exceptions. This last part is desired
   * because we save the FPU state on each task switch. */
  asm volatile("clts\n"
               "mov %%cr0, %0"
               : "=r"(cr));
  cr &= ~CR0_EM;
  cr |= CR0_MP;
  asm volatile("mov %0, %%cr0" ::"r"(cr));
  /* Configure CR4: enable the `fxsave` and `fxrstor` instructions, along
   * with SSE, and SSE-related exception handling. */
  asm volatile("mov %%cr4, %0" : "=r"(cr));
  cr |= CR4_OSFXSR | CR4_OSXMMEXCPT;

  asm volatile("mov %0, %%cr4\n"
               "fninit" ::"r"(cr));
}

void fpu_kernel_enter() {
  asm volatile("fxsave (%0)\n"
               "fninit\n" ::"r"(kernel_fpu));
}

void fpu_kernel_exit() { asm volatile("fxrstor (%0)" ::"r"(kernel_fpu)); }