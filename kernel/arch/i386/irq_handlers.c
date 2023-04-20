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
#include <kernel/keyboard.h>
#include <kernel/pit.h>
#include <kernel/tty_framebuffer.h>
#include <kernel/config.h>

#if GUI_MODE
#include <kernel/gui.h>
#endif

extern uint32_t simple_sc_to_kc[];
static uint32_t device;
static kbd_context_t context;
static void *irq_routines[16] = {0};
extern char keyboard_us[];
extern char keyboard_us_shift[];
bool key_pressed[128];
int8_t  mouse_byte[3];

uint8_t mouse_cycle = 0;


void irq_register_handler(int irq, void (*handler)(registers_t*)){

    if((NULL == handler) || (irq<0) || (irq>15)) return;
    irq_routines[irq] = handler;

}

void irq_unregister_handler(int irq){

    irq_routines[irq] = 0;

}

void handle_platform_irq(registers_t* frame){

    void (*handler)(registers_t* frame);
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


/*void sys_sleep(int seconds){
    log(LOG_SERIAL, false, "seconds to wait : %d\n", seconds);
    uint64_t sleep_ticks = ticks + (seconds * SYSTEM_TICKS_PER_SEC);
    log(LOG_SERIAL, false, "start tick : %d\n", ticks);
    log(LOG_SERIAL, false, "end tick : %d\n", sleep_ticks);
    while(ticks < sleep_ticks){
        log(LOG_SERIAL, false, "tick : %d\n", ticks);
    }
}*/
void sys_mouse_handler(registers_t* frame){
    log(LOG_SERIAL, false, "mouse moved\n");
    return;
    //(void)frame;
    /*uint8_t status = inb(MOUSE_STATUS);
    while (status & MOUSE_BBIT){
        int8_t mouse_in = inb(MOUSE_PORT);
        if (status & MOUSE_F_BIT){
            switch(mouse_cycle){
                case 0:
                    mouse_byte[0] = mouse_in;
                    ++mouse_cycle;
                    break;
                case 1:
                    mouse_byte[1] = mouse_in;
					++mouse_cycle;
					break;
                case 2:
                    mouse_byte[2] = mouse_in;
                    if (mouse_byte[0] & 0x80 || mouse_byte[0] & 0x40) {
						// x/y overflow? bad packet! 
						break;
					}
                    log(LOG_SERIAL, false, "mouse packet\n");
            }
        }
    }*/

}


void sys_key_handler(registers_t* frame){
    (void)frame;
    // scan code https://wiki.osdev.org/PS/2_Keyboard
    // https://github.com/29jm/SnowflakeOS/blob/master/kernel/src/devices/kbd.c
    uint8_t scan_code = inb(0x60);
    if (scan_code == 0xE0){
        return; // shift pressed
    }
    //uint8_t scan_code = ps2_read(PS2_DATA);
    //log(LOG_SERIAL, false, "scan code : %d\n", scan_code);
    bool pressed = 1;
    if (scan_code >= 128){
        pressed = false;
        scan_code -= 128;
    }
    key_pressed[scan_code] = pressed;
    if (!pressed) {
        return;
    }

#if GUI_MODE
    log(LOG_SERIAL, false, "key pressed : %d\n", scan_code);
    if (scan_code & 0x80){
        log(LOG_SERIAL, false, "modifer key/special key pressed : %d\n", scan_code);
    } else if (scan_code == 0x4B){
        log(LOG_SERIAL, false, "left\n");
        move_focused_window_wm(LEFT);
    } else if (scan_code == 0x48){
        log(LOG_SERIAL, false, "up\n");
        move_focused_window_wm(UP);
    } else if (scan_code == 0x4D){
        log(LOG_SERIAL, false, "right\n");
        move_focused_window_wm(RIGHT);
    } else if (scan_code == 0x50){
        log(LOG_SERIAL, false, "down\n");
        move_focused_window_wm(DOWN);
    } else if (scan_code < 0x81){
        char c = keyboard_us[scan_code];
        gui_keypress(c);
    }

#else
    if(scan_code == ESC_KEY){ // ESC - pressed
        reboot();
    }
    if (scan_code == ENTER_KEY){ // ENTER - pressed
        //launch_command();
        launch_command_framebuffer();
        empty_line_cli_framebuffer();
        //empty_line_cli();
        printf("\n> ");
    } else if (scan_code == DELETE_KEY || scan_code == BACKSPACE_KEY){ // DELETE - pressed
        write_serialf("delete pressed\n");
        terminal_framebuffer_delete_character();
        //remove_character();
        //bug with delete character (wrong keycode)
    } else if (scan_code == CURSOR_LEFT_KEY) {
        move_cursor_left();
    } else if (scan_code == CURSOR_UP_KEY){
        //terminal_print_last_command();
    } else if (scan_code < 0x81){
        //terminal_keypress(scan_code);
        char c = keyboard_us[scan_code];
        terminal_framebuffer_keypress(c);
    }
#endif
}