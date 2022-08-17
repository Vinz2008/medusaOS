#include <stdarg.h>
#include <limits.h>
#include <kernel/io.h>
#include <kernel/serial.h>
#include <string.h>

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
   	int written = 0;
   	while (*format != '\0') {
		size_t maxrem = INT_MAX - written;

		if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%')
				format++;
			size_t amount = 1;
			while (format[amount] && format[amount] != '%')
				amount++;
			if (maxrem < amount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			write_serial_with_size(format, amount);
			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format++;

		if (*format == 'c') {
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			if (!maxrem) {
				// TODO: Set errno to EOVERFLOW.
				return;
			}
			write_serial_with_size(&c, sizeof(c));
			written++;
		} else if (*format == 's') {
			format++;
			const char* str = va_arg(parameters, const char*);
			size_t len = strlen(str);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return;
			}
			
			write_serial(str);
			written += len;
		} else if (*format == 'i') {
			format++;
			char i2[10];
			memset(i2, 0, 10);
			int i = va_arg(parameters, int);
			int_to_ascii(i, i2);
			if (!maxrem) {
				// TODO: Set errno to EOVERFLOW.
				return;
			}
			write_serial(&i2);
			written++;
		} else if (*format == 'd') {
			format++;
			char i2[10];
			memset(i2, 0, 10);
			//char* i2;
			int i = va_arg(parameters, int);
			int_to_ascii(i, i2);
			if (!maxrem) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			write_serial(&i2);
			written++;
		} else if (*format == 'p'){
			format++;
			char i2[10];
			memset(i2, 0, 10);
			void* i = va_arg(parameters, void*);
			hex_to_ascii(i, i2);
			if (!maxrem) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			write_serial(&i2);
			written++;
		} else {
			format = format_begun_at;
			size_t len = strlen(format);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return;
			}
			write_serial(format);
			written += len;
			format += len;
		}
	}

	va_end(parameters);
}