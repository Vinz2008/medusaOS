#include <kernel/x86.h>

void proc_enter_usermode(){
    x86_disable_int();
    
}