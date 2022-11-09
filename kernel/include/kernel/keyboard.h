#include <types.h>

#define KBD_SSC_CMD 0xF0
#define KBD_SSC_GET 0x00
#define KBD_SSC_2 0x02
#define KBD_SSC_3 0x03

uint8_t read_scan_code(void);
void init_keyboard(uint32_t dev);