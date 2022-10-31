#include <types.h>
#include <string.h>
#include <stdlib.h>
#include <kernel/io.h>
#include <kernel/gui.h>
#include <kernel/x86.h>

#define PALETTE_MASK 0x3C6
#define PALETTE_READ 0x3C7
#define PALETTE_WRITE 0x3C8
#define PALETTE_DATA 0x3C9

uint8_t white_color = COLOR(0,0,0);

static uint8_t* buffer_memory = (uint8_t*) 0xA0000; 
uint8_t backbuffer[2][SCREEN_SIZE];
uint8_t sback = 0;
#define CURRENT (backbuffer[sback])
#define SWAP() (sback = 1 - sback)

void fill_screen_gui(uint8_t color){
    memset(&CURRENT, color, SCREEN_SIZE);
}

void clear_screen_gui(){
    memset(&CURRENT, white_color, SCREEN_SIZE);
}

void swap_screen_buffer(){
    memcpy(buffer_memory, &CURRENT, SCREEN_SIZE);
    SWAP();
}

void init_gui(){
    outb(PALETTE_MASK, 0xFF);
    outb(PALETTE_WRITE, 0);
    for (uint8_t i = 0; i < 255; i++){
        outb(PALETTE_DATA, (((i >> 5) & 0x7) * (256 / 8)) / 4);
        outb(PALETTE_DATA, (((i >> 2) & 0x7) * (256 / 8)) / 4);
        outb(PALETTE_DATA, (((i >> 0) & 0x3) * (256 / 4)) / 4);
    }
    outb(PALETTE_DATA, 0x3F);
    outb(PALETTE_DATA, 0x3F);
    outb(PALETTE_DATA, 0x3F);
    clear_screen_gui();
    x86_halt();
    while (1){}
    abort();
    /*while (true){
        swap_screen_buffer();
    }*/
}