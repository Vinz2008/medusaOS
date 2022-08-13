#include <kernel/io.h>

void reboot(){
    outb(0x64, 0xFE);
}