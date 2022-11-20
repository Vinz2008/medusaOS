#include <stdio.h>
#include <stdarg.h>
#include <kernel/tty.h>
#include <kernel/tty_framebuffer.h>

void alert(const char* restrict format, ...){
    //terminal_setcolors(7, 4);
    terminal_framebuffer_setcolor(RED);
    va_list parameters;
	va_start(parameters, format);
    vprintf(format, parameters);
    //terminal_reset_color();
    terminal_framebuffer_reset_color();
}

void green(const char* restrict format, ...){
    //terminal_setcolor(2);
    terminal_framebuffer_setcolor(GREEN);
    va_list parameters;
	va_start(parameters, format);
    vprintf(format, parameters);
    //terminal_reset_color();
    terminal_framebuffer_reset_color();
}


// for having ok printed in green when booting

void ok_printing_boot(const char* restrict format, ...){
    va_list parameters;
	va_start(parameters, format);
    vok_printing_boot(format, parameters);
}

void vok_printing_boot(const char* restrict format, va_list parameters){
    green("[ OK ] ");
    vprintf(format, parameters);
}