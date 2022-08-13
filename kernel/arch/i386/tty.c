#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <kernel/tty.h>
#include <kernel/io.h>
#include "vga.h"


extern char keyboard_us[];

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint8_t old_terminal_color;
static uint16_t* terminal_buffer;
static size_t terminal_keypress_index = 0;
static size_t terminal_tick_index = 0;
int IsBacklash = false;


void enable_cursor(uint8_t cursor_start, uint8_t cursor_end){
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}


void disable_cursor(){
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}


void update_cursor(int x, int y){
	uint16_t pos = y * VGA_WIDTH + x;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

uint16_t get_cursor_position(void){
    uint16_t pos = 0;
    outb(0x3D4, 0x0F);
    pos |= inb(0x3D5);
    outb(0x3D4, 0x0E);
    pos |= ((uint16_t)inb(0x3D5)) << 8;
    return pos;
}


void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

	terminal_buffer = VGA_MEMORY;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color){
	old_terminal_color = terminal_color;
	terminal_color = color;
}

void terminal_setcolors(uint8_t fg, uint8_t bg){
	old_terminal_color = terminal_color;
	terminal_color = vga_entry_color(fg, bg);
}

void terminal_reset_color(){
	terminal_color = old_terminal_color;
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
	unsigned char uc = c;
	if (uc == '\n'){
		terminal_row++;
		terminal_column = 0;
	}else {
	terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
	}
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT) {
			terminal_row = 0;
		}
	}
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++) {
		terminal_putchar(data[i]);
	}

}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}


void terminal_keypress_init(uint8_t n){
	terminal_keypress_index = VGA_WIDTH * terminal_row + n;
}


void terminal_tick_init(uint8_t n){
	terminal_tick_index = VGA_WIDTH * terminal_row + n;
}

void terminal_tick(char c){
	terminal_buffer[terminal_tick_index] = vga_entry(c,terminal_color);
}

void terminal_keypress(uint8_t scan_code){
	char c = keyboard_us[scan_code];
	terminal_buffer[terminal_keypress_index] = vga_entry(c,terminal_color);
}