#include "kernel/io.h"
#include "kernel/serial.h"


void serial_configure_baud_rate(uint16_t port, uint16_t divider){
    outb(SERIAL_LINE_COMMAND_PORT(port), SERIAL_LINE_ENABLE_DLAB);
    outb(SERIAL_DATA_PORT(port), (divider >> 8) & 0x00FF);
    outb(SERIAL_DATA_PORT(port), divider & 0x00FF);
}

void serial_configure_line(uint16_t port){
    outb(SERIAL_LINE_COMMAND_PORT(port), 0x03);
}

int serial_is_transmit_fifo_empty(uint16_t port){
    return inb(SERIAL_LINE_STATUS_PORT(port)) & 0x20;
}


int serial_init(uint16_t port){
    serial_configure_baud_rate(port, 0x03);
    outb(SERIAL_DATA_PORT(port), 0xAE);
    if(inb(port + 0) != 0xAE) {
      return 1;
   }
   outb(SERIAL_MODEM_COMMAND_PORT(port), 0x0F);
   return 0;
}


int serial_received(uint16_t port) {
   return inb(SERIAL_LINE_STATUS_PORT(port)) & 1;
}


char read_serial(uint16_t port) {
   while (serial_received(port) == 0);
 
   return inb(port);
}

int is_transmit_empty(uint16_t port) {
   return inb(SERIAL_LINE_STATUS_PORT(port)) & 0x20;
}
 
void write_serial(char a, uint16_t port) {
   while (is_transmit_empty(port) == 0);
 
   outb(port,a);
}