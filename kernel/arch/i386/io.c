#include <types.h> 
 
 
void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" :: "a" (value), "dN" (port));
}

uint8_t inb (uint16_t port) {
    uint8_t result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

inline void io_wait(void){
    outb(0x80, 0);
}