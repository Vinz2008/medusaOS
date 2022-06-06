#include <types.h>
#include "kernel/io.h"
#define	PIT_REG_COMMAND	0x43
#define	PIT_OCW_COUNTER_0 0	
#define	PIT_OCW_COUNTER_1 0x40	
#define	PIT_REG_COUNTER0 0x40
#define PIT_REG_COUNTER1 0x41
#define	PIT_REG_COUNTER2 0x42	
#define	PIT_OCW_MASK_MODE 0xE
#define	PIT_OCW_MASK_RL	0x30
#define	PIT_OCW_RL_DATA	0x30
#define PIT_OCW_MASK_COUNTER 0xC0

uint32_t _pit_ticks=0;

void i86_pit_send_command(uint8_t cmd){
	outb(PIT_REG_COMMAND, cmd);
}


void interrupt _cdecl i86_pit_irq() {
 
	//! macro to hide interrupt start code
	//intstart ();
 
	//! increment tick count
	_pit_ticks++;
 
	//! tell hal we are done
	//interruptdone(0);
 
	//! macro used with intstart to return from interrupt handler
	//intret ();
}


void i86_pit_send_data (uint16_t data, uint8_t counter) {
 
	uint8_t	port= (counter==PIT_OCW_COUNTER_0) ? PIT_REG_COUNTER0 :
		((counter==PIT_OCW_COUNTER_1) ? PIT_REG_COUNTER1 : PIT_REG_COUNTER2);
 
	outportb (port, data);
}
 
//! read data from counter
uint8_t i86_pit_read_data (uint16_t counter) {
 
	uint8_t	port= (counter==PIT_OCW_COUNTER_0) ? PIT_REG_COUNTER0 :
		((counter==PIT_OCW_COUNTER_1) ? PIT_REG_COUNTER1 : PIT_REG_COUNTER2);
 
	return inportb (port);
}


void i86_pit_start_counter (uint32_t freq, uint8_t counter, uint8_t mode) {
 
	if (freq==0)
		return;
 
	uint16_t divisor = 1193180 / freq;
 
	//! send operational command
	uint8_t ocw=0;
	ocw = (ocw & ~PIT_OCW_MASK_MODE) | mode;
	ocw = (ocw & ~PIT_OCW_MASK_RL) | PIT_OCW_RL_DATA;
	ocw = (ocw & ~PIT_OCW_MASK_COUNTER) | counter;
	i86_pit_send_command (ocw);
 
	//! set frequency rate
	i86_pit_send_data (divisor & 0xff, 0);
	i86_pit_send_data ((divisor >> 8) & 0xff, 0);
 
	//! reset tick count
	_pit_ticks=0;
}