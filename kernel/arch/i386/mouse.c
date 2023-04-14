#include <kernel/mouse.h>
#include <stdio.h>
#include <kernel/irq_handlers.h>
#include <kernel/x86.h>
#include <kernel/io.h>

uint8_t ack;

void mouse_wait(uint8_t a_type) {
    uint32_t _time_out=100000; //unsigned int
    if(a_type == 0) {
        while(_time_out--) {
            if((inb(0x64) & 1)==1)
            {
                return;
            }
        }
        return;
    }
    else {
        while(_time_out--) {
            if((inb(0x64) & 2)==0) {
                return;
            }
        }
        return;
    }
}

void mouse_write(uint8_t write) {
	outb(MOUSE_STATUS, MOUSE_WRITE);
	outb(MOUSE_PORT, write);
}

uint8_t mouse_read() {
	char t = inb(MOUSE_PORT);
	return t;
}

void mouse_install(){
    // https://wiki.osdev.org/PS/2_Mouse
    // https://github.com/szhou42/osdev/blob/master/src/kernel/drivers/mouse.c
    uint8_t _status;
    mouse_wait(1);
    outb(0x64, 0x20);
    mouse_wait(0);
    _status=(inb(0x60) | 2);
    mouse_wait(1);
    outb(0x64, 0x60);
    mouse_wait(1);
    outb(0x60, _status);
    mouse_write(0xF6);
    mouse_read();
    mouse_write(0xF4);
    mouse_read();
    /*uint8_t status;
    outb(MOUSE_STATUS, 0xA8);
    outb(MOUSE_STATUS, 0x20);
    status = inb(MOUSE_PORT) | 2;
    outb(MOUSE_STATUS, 0x60);
    outb(MOUSE_PORT, status);
    mouse_write(0xF6);
    mouse_read();
	mouse_write(0xF4);
	mouse_read();*/
    irq_register_handler(12, sys_mouse_handler);
    log(LOG_SERIAL, true, "IRQ handler set: sys_mouse_handler\n");
}