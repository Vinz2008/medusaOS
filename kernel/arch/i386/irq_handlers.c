#include <kernel/irq_handlers.h>
#include <types.h>
#include <stdio.h>
#include <stddef.h>
#include <kernel/io.h>
#include <kernel/x86.h>
#include <kernel/ps2.h>
#include <kernel/pic.h>
#include <kernel/misc.h>
#include <kernel/tty.h>
#include <kernel/serial.h>
#include <kernel/mouse.h>
#include <kernel/pit.h>
#include <kernel/tty.h>

extern bool tick_animation_enabled;

static void *irq_routines[16] = {0};

void irq_register_handler(int irq, void (*handler)(x86_iframe_t*)){

    if((NULL == handler) || (irq<0) || (irq>15)) return;
    irq_routines[irq] = handler;

}

void irq_unregister_handler(int irq){

    irq_routines[irq] = 0;

}

void handle_platform_irq(x86_iframe_t* frame){

    void (*handler)(x86_iframe_t* frame);
    uint32_t irq = frame->vector -32;

    handler = irq_routines[irq];

    if (handler){
        handler(frame);
    }

    if(irq==IRQ_PIT){
        return;
    }

    pic_send_EOI(irq);
}

void sys_tick_handler(x86_iframe_t* frame){
    ++ticks;
    if (tick_animation_enabled == true){
    const char ticks_anim_chars[] = {'-', '/', '|', '\\'}; 
    size_t ti = ticks%4;
    terminal_tick(ticks_anim_chars[ti]);
    }
    pic_send_EOI(IRQ_PIT);
}

void sys_sleep(int seconds){
    log(LOG_SERIAL, false, "seconds to wait : %d\n", seconds);
    uint64_t sleep_ticks = ticks + (seconds * SYSTEM_TICKS_PER_SEC);
    log(LOG_SERIAL, false, "start tick : %d\n", ticks);
    log(LOG_SERIAL, false, "end tick : %d\n", sleep_ticks);
    while(ticks < sleep_ticks){
        log(LOG_SERIAL, false, "tick : %d\n", ticks);
    }
}


void sys_key_handler(x86_iframe_t* frame){
    // scan code https://wiki.osdev.org/PS/2_Keyboard
    uint8_t scan_code = inb(0x60);
    if(scan_code == ESC_KEY){ // ESC - pressed
        reboot();
    }
    if (scan_code == ENTER_KEY){ // ENTER - pressed
        launch_command();
        empty_line_cli();
        printf("\n> ");
    } else if (scan_code == DELETE_KEY){ // DELETE - pressed
        //write_serialf("delete pressed\n");
        //remove_character();
        //bug with delete character (wrong keycode)
    } else if (scan_code == CURSOR_LEFT_KEY) {
        move_cursor_left();
    } else if (scan_code == CURSOR_UP_KEY){
        //terminal_print_last_command();
    } else if (scan_code < 0x81){
        terminal_keypress(scan_code);
    }
}