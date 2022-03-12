#include <types.h>
struct GDT
{
   uint16_t limit;
   uint32_t base;
   uint16_t access_byte;
   uint16_t flags;
};


void encodeGdtEntry(uint8_t *target, struct GDT source);
void CreatingDescriptorsStart();
void create_descriptor(uint32_t base, uint32_t limit, uint16_t flag);