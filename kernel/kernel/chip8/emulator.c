#include <stdbool.h>
#include <kernel/irq_handlers.h>
#include <kernel/graphics.h>
#include <kernel/fb.h>

#define CHIP8_SCREEN_WIDTH 54
#define CHIP8_SCREEN_HEIGHT 32

bool chip8_emulator_mode = false;

static fb_t fb;

extern char keyboard_us[];

int pc;

uint8_t ram[4096];



bool display[CHIP8_SCREEN_WIDTH*CHIP8_SCREEN_HEIGHT];

void set_chip8_emulator_mode(bool mode){
    chip8_emulator_mode = mode;
}

bool is_chip8_emulator_mode(){
    return chip8_emulator_mode;
}

void write_chip8_screen(uint8_t x, uint8_t y){
    for (int xtemp = 0; xtemp < CHIP8_SCREEN_WIDTH; xtemp++){
        for (int ytemp = 0; ytemp < CHIP8_SCREEN_HEIGHT; ytemp++){
            //draw_pixel
        }
    }
}

void chip8_keyboard_handler(int scan_code){
    if (scan_code == ENTER_KEY){

    } else if (scan_code < 0x81){
        char c = keyboard_us[scan_code];
    }
}

void chip8_mainloop(uint8_t){
    
}

void setup_chip8_emulator(){
    fb = fb_get_info();
    setup_custom_keypress_function(chip8_keyboard_handler);
    fill_screen(fb, COLOR_BLUE);
}