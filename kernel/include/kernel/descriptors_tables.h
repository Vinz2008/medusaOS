// I looked at this to implement it https://github.com/ozkl/soso/blob/master/kernel/descriptortables.h

#include <types.h>


void descriptor_tables_initialize();


struct GdtEntry
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

typedef struct GdtEntry GdtEntry;


struct GdtPointer
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

typedef struct GdtPointer GdtPointer;

