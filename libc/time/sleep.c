#include <time.h>
#include <kernel/time.h>

void sleep(int seconds){
    sys_sleep(seconds);
}