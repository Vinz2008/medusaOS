#include <kernel/time.h>
#include <time.h>

void sleep(int seconds) { sys_sleep(seconds); }