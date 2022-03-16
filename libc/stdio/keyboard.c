#include <stdio.h>

unsigned char read_scan_code(void)
{
        return inb(KBD_DATA_PORT);
}