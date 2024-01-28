#include <types.h>

#ifndef _MISC_HEADER_
#define _MISC_HEADER_

void reboot();

static uint32_t align_to(uint32_t n, uint32_t align) {
  if (n % align == 0) {
    return n;
  }

  return n + (align - n % align);
}

static uint32_t divide_up(uint32_t n, uint32_t d) {
  if (n % d == 0) {
    return n / d;
  }

  return 1 + n / d;
}

#endif