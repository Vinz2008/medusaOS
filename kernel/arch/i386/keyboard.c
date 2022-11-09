#include <kernel/keyboard.h>
#include <types.h>
#include <stdio.h>
#include <kernel/irq_handlers.h>
#include <kernel/ps2.h>
#include <kernel/io.h>

static uint32_t device;

void init_keyboard(uint32_t dev){
  device = dev;

  irq_register_handler(1, sys_key_handler);
  log(LOG_ALL, true, "IRQ handler set: sys_key_handler\n");	
  ps2_write_device(device, KBD_SSC_CMD);
  ps2_expect_ack();
  ps2_write_device(device, KBD_SSC_GET);
  ps2_expect_ack();
  uint8_t scancode_set = ps2_read(PS2_DATA);
  if (scancode_set != KBD_SSC_2) {
    log(LOG_SERIAL, false, "wrong scancode set (%d), TODO", scancode_set);
  }
  ps2_write_device(device, PS2_DEV_ENABLE_SCAN);
  ps2_expect_ack();
}

#define KBD_DATA_PORT   0x60


uint8_t read_scan_code(void){
    return inb(KBD_DATA_PORT);
}

char keyboard_us [128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   
  '\t', /* <-- Tab */
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',     
    0, /* <-- control key */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};