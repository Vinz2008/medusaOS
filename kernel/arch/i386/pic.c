#include <types.h>
#include <kernel/io.h>

#define PIC1_PORT_A 0x20
#define PIC2_PORT_A 0xA0
#define PIC1_START_INTERRUPT 0x20
#define PIC2_START_INTERRUPT 0x28
#define PIC1_REG_DATA 0x21
#define PIC2_REG_DATA 0xA1
#define PIC2_END_INTERRUPT PIC2_START_INTERRUPT + 7
#define PIC_ACK 0x20
#define PIC_ICW1_MASK_IC4 0x1
#define PIC_ICW1_INIT 0x10
#define PIC_ICW1_IC4_EXPECT 1
#define PIC_ICW4_MASK_UPM 0x1
#define PIC_ICW4_UPM_86MODE		1
	

void pic_acknowledge(unsigned int interrupt)
{
    if (interrupt < PIC1_START_INTERRUPT || interrupt > PIC2_END_INTERRUPT) {
        return;
    }

    if (interrupt < PIC2_START_INTERRUPT) {
        outb(PIC1_PORT_A, PIC_ACK);
    } else {
        outb(PIC2_PORT_A, PIC_ACK);
    }
}

inline void i86_pic_send_command(uint8_t cmd, uint8_t picNum) {
 
	if (picNum > 1)
		return;
 
	uint8_t	reg = (picNum==1) ? PIC2_PORT_A : PIC1_START_INTERRUPT;
	outb(reg, cmd);
}

inline void i86_pic_send_data(uint8_t data, uint8_t picNum) {
	if (picNum > 1)
		return;
	uint8_t	reg = (picNum==1) ? PIC2_REG_DATA : PIC1_REG_DATA;
	outb(reg, data);
}
 
inline uint8_t i86_pic_read_data(uint8_t picNum) {
	if (picNum > 1)
		return 0;
	uint8_t	reg = (picNum==1) ? PIC2_REG_DATA : PIC1_REG_DATA;
	return inb(reg);
}


void i86_pic_initialize(uint8_t base0, uint8_t base1) {
	uint8_t icw	= 0;
	//! Begin initialization of PIC
	icw = (icw & ~PIC_ICW1_INIT) | PIC_ICW1_INIT;
	icw = (icw & ~PIC_ICW1_MASK_IC4) | PIC_ICW1_IC4_EXPECT;
	i86_pic_send_command(icw, 0);
	i86_pic_send_command(icw, 1);
	//! Send initialization control word 2. This is the base addresses of the irq's
	i86_pic_send_data(base0, 0);
	i86_pic_send_data(base1, 1);
	//! Send initialization control word 3. This is the connection between master and slave.
	//! ICW3 for master PIC is the IR that connects to secondary pic in binary format
	//! ICW3 for secondary PIC is the IR that connects to master pic in decimal format
	i86_pic_send_data(0x04, 0);
	i86_pic_send_data(0x02, 1);
	//! Send Initialization control word 4. Enables i86 mode
	icw = (icw & ~PIC_ICW4_MASK_UPM) | PIC_ICW4_UPM_86MODE;
 
	i86_pic_send_data(icw, 0);
	i86_pic_send_data(icw, 1);
}

