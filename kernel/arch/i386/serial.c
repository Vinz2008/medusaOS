#include <stdarg.h>
#include <limits.h>
#include <kernel/io.h>
#include <kernel/serial.h>
#include <kernel/vfs.h>
#include <string.h>
#include <stdio.h>

int init_serial(){
   outb(PORT + 1, 0x00);
   outb(PORT + 3, 0x80);
   outb(PORT + 0, 0x03);
   outb(PORT + 1, 0x00);
   outb(PORT + 3, 0x03);
   outb(PORT + 2, 0xC7);
   outb(PORT + 4, 0x0B);
   outb(PORT + 4, 0x1E);
   outb(PORT + 0, 0xAE);
   if (inb(PORT + 0) != 0xAE){
      return 1;
   }
   outb(PORT + 4, 0x0F);
   return 0;
}

int serial_received(){
   return inb(PORT + 5) & 1;
}


char read_serial(){
   while (serial_received() == 0);
 
   return inb(PORT);
}

int is_transmit_empty(){
   return inb(PORT + 5) & 0x20;
}
 
void write_serial_char(char a){
   while (is_transmit_empty() == 0);
 
   outb(PORT,a);
}


void write_serial(const char* str){
   for (int i = 0; i < strlen(str); i++){
      write_serial_char(str[i]);
   }
}


void write_serial_with_size(const char* data, size_t length){
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
		write_serial_char(bytes[i]);
}

void write_serialf(const char* restrict format, ...){
	va_list parameters;
	va_start(parameters, format);
	vwrite_serialf(format, parameters);
}


void vwrite_serialf(const char* restrict format, va_list parameters){
   	vfprintf(VFS_FD_SERIAL, format, parameters);
	va_end(parameters);
}