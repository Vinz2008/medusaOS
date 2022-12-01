#include <types.h>

#define SYSTEM_TICKS_PER_SEC 1000

void pit_init(uint32_t frequency);
void pit_mdelay(uint64_t ms);