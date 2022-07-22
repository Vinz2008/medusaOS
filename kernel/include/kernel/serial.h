#define PORT 0x3f8 

int init_serial();

void write_serial(const char* str);
void write_serialf(const char* restrict format, ...);