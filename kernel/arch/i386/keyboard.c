#include <stdio.h>
#include "kernel/io.h"

#define KBD_DATA_PORT   0x60
uint8_t read_scan_code(void)
{
        return inb(KBD_DATA_PORT);
}