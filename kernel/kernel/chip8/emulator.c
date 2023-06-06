#include <kernel/chip8_emulator.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel/irq_handlers.h>
#include <kernel/graphics.h>
#include <kernel/fb.h>
#include <kernel/vfs.h>
#include <kernel/stack.h>

bool chip8_emulator_mode = false;

static fb_t fb;

extern char keyboard_us[];

static int pc;

static chip8_emulator_t emulator;

uint32_t chip8_int_to_hex(int num, int base, char sign){
    char outbuf[10];
    memset(outbuf, 0, 10);
    int i = 12;
    int j = 0;

    do{
        outbuf[i] = "0123456789ABCDEF"[num % base];
        i--;
        num = num/base;
    }while( num > 0);

    if(sign != ' '){
        outbuf[0] = sign;
        ++j;
    }

    while( ++i < 13){
       outbuf[j++] = outbuf[i];
    }

    outbuf[j] = 0;
    return strtol(outbuf, NULL, base);
}

void set_chip8_emulator_mode(bool mode){
    chip8_emulator_mode = mode;
}

bool is_chip8_emulator_mode(){
    return chip8_emulator_mode;
}

void write_chip8_pixel(uint8_t x, uint8_t y, bool state){
    for (int xtemp = 0; xtemp < fb.width/CHIP8_SCREEN_WIDTH; xtemp++){
        for (int ytemp = 0; ytemp < fb.height/CHIP8_SCREEN_HEIGHT; ytemp++){
            draw_pixel(fb, x* fb.width/CHIP8_SCREEN_WIDTH + xtemp, y * fb.height/CHIP8_SCREEN_HEIGHT + ytemp, (state) ? COLOR_RED : COLOR_BLACK);
        }
    }
}

void chip8_update_framebuffer(){
    for (int xtemp = 0; xtemp < CHIP8_SCREEN_WIDTH; xtemp++){
        for (int ytemp = 0; ytemp < CHIP8_SCREEN_HEIGHT; ytemp++){
            uint8_t index = xtemp + (ytemp*CHIP8_SCREEN_WIDTH);
            bool state = emulator.display[index];
            write_chip8_pixel(xtemp, ytemp, state);
        }
    }
}

void write_chip8_screen(uint8_t x, uint8_t y, bool state){
    if (x > CHIP8_SCREEN_WIDTH){
        x -= CHIP8_SCREEN_WIDTH;
    } else if (x < 0){
        x += CHIP8_SCREEN_WIDTH;
    }
    if (y > CHIP8_SCREEN_HEIGHT){
        y -= CHIP8_SCREEN_HEIGHT;
    } else if (y < 0){
        y += CHIP8_SCREEN_HEIGHT;
    }
    uint8_t index = x + (y * CHIP8_SCREEN_WIDTH);
    emulator.display[index] = state;
}

void fill_chip8_screen(){
    for (int xtemp = 0; xtemp < CHIP8_SCREEN_WIDTH; xtemp++){
        for (int ytemp = 0; ytemp < CHIP8_SCREEN_HEIGHT; ytemp++){
            if (xtemp % 2 == 0 && ytemp % 2 == 0){
                write_chip8_pixel(xtemp, ytemp, true);
                //write_chip8_screen(xtemp, ytemp, true);
            }
        }
    }
}

void chip8_keyboard_handler(int scan_code){
    if (scan_code == ENTER_KEY){

    } else if (scan_code < 0x81){
        char c = keyboard_us[scan_code];
    }
}

uint16_t chip8_get_combined_opcode(uint8_t opcode_1, uint8_t opcode_2){
    uint16_t result = (opcode_1 << 4) | opcode_2;
    return result;
}

void chip8_mainloop(uint8_t opcode_1, uint8_t opcode_2){
    emulator.pc += 2;
    log(LOG_SERIAL, false, "chip8_get_combined_opcode(0x00, 0xE0) : %x\n", chip8_get_combined_opcode(0x00, 0xE0));
    log(LOG_SERIAL, false, "opcode 1 (in decimal) : %d, opcode 1 >> 4 : %x, opcode 1 & 0x0F : %x\n", opcode_1, opcode_1 >> 4, opcode_1 & 0x0F);
    // first number : opcode >> 4
    // second number : opcode & 0x0F
    
    switch (opcode_1 >> 4){
        case 0x0:
            if (opcode_2 == 0xE0){
                fill_screen(fb, COLOR_BLACK);
            } else if (opcode_2 == 0xEE){
                // return from subroutine
                stack_pop(&emulator.stack);
            }
            break; 
        case 0x1:
            break;
        case 0x2:
            break;
        case 0x3:
            break;
        case 0x4:
            break;
        case 0x5:
            break;
        case 0x6:
            break;
        case 0x7:
            break;
        case 0x8:
            break;
        case 0x9:
            break;
    }
}

void chip8_load_program_into_ram(uint8_t* buf, size_t buf_length){
    for (int i = 0; i < buf_length; i++){
        emulator.ram[0x200 + i] = buf[i];
    }
}

void setup_chip8_emulator(const char* filename){
    set_chip8_emulator_mode(true);
    emulator.pc = 0x200;
    emulator.speed = 10;
    emulator.paused = false;
    memset(emulator.display, false, CHIP8_SCREEN_WIDTH*CHIP8_SCREEN_HEIGHT);

    fb = fb_get_info();


    setup_custom_keypress_function(chip8_keyboard_handler);
    fill_screen(fb, COLOR_BLUE);
    log(LOG_SERIAL, false, "TEST ROM");
    fs_node_t* rom_node = vfs_open(filename, "r");
    uint8_t buffer[rom_node->length];
    log(LOG_SERIAL, false, "rom_node->length : %d\n", rom_node->length);
    if (rom_node == NULL){
        log(LOG_SERIAL, false, "rom file not found : %s\n", filename);
    }
    read_fs(rom_node, 0, rom_node->length, buffer);
    for (int i = 0; i < rom_node->length; i++){
        log(LOG_SERIAL, false, "chip instruction (in decimal) : %d\n", buffer[i]);
    }
    chip8_load_program_into_ram(buffer, rom_node->length);
    for (pc = 0; pc < rom_node->length; pc+=2){
        chip8_mainloop(buffer[pc], buffer[pc+1]);
    }
    fill_chip8_screen();
    //chip8_update_framebuffer();
}