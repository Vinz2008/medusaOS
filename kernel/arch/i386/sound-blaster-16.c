#include <stddef.h>
#include <kernel/io.h>
#include <kernel/irq_handlers.h>

#define RESET_PORT 0x226
#define TURN_ON_PORT 0x22C
#define MIXER_IRQ 0x5
#define MIXER_IRQ_DATA  0x2
#define DSP_MIXER 0x224
#define DSP_IRQ     0x80
#define DSP_MIXER_DATA  0x225

void reset_sound_blaster_16(){
    outb(RESET_PORT, 1);
    // TODO: maybe not necessary
    // ~3 microseconds?
    for (size_t i = 0; i < 1000000; i++);
    outb(RESET_PORT, 0);
}

void sound_blaster_16_irq_handler(x86_iframe_t* frame){

}

void configure_sound_blaster_16(){
    irq_register_handler(MIXER_IRQ, sound_blaster_16_irq_handler);
    outb(DSP_MIXER, DSP_IRQ);
    outb(DSP_MIXER_DATA, MIXER_IRQ_DATA);
}


void init_sound_blaster_16(){
    irq_register_handler(MIXER_IRQ, sound_blaster_16_irq_handler);
    reset_sound_blaster_16();
    configure_sound_blaster_16();
    
    outb(TURN_ON_PORT, 0xD1);

}