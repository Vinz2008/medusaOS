#include <kernel/mouse.h>
#include <stdio.h>
#include <kernel/irq_handlers.h>
#include <kernel/x86.h>


void mouse_write(uint8_t write) {
	outb(MOUSE_STATUS, MOUSE_WRITE);
	outb(MOUSE_PORT, write);
}

uint8_t mouse_read() {
	char t = inb(MOUSE_PORT);
	return t;
}

void mouse_install(){
    uint8_t status;
    outb(MOUSE_STATUS, 0xA8);
    outb(MOUSE_STATUS, 0x20);
    status = inb(MOUSE_PORT) | 2;
    outb(MOUSE_STATUS, 0x60);
    outb(MOUSE_PORT, status);
    mouse_write(0xF6);
    mouse_read();
	mouse_write(0xF4);
	mouse_read();
    irq_register_handler(12, sys_mouse_handler);
    log(LOG_SERIAL, true, "IRQ handler set: sys_mouse_handler\n");
}