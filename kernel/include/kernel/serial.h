
#define SERIAL_COM1_BASE 0x3F8
#define SERIAL_DATA_PORT(base) (base)
#define SERIAL_FIFO_COMMAND_PORT(base) (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base) (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base) (base + 5)
#define SERIAL_LINE_ENABLE_DLAB         0x80


int serial_init(uint16_t port);

int serial_received(uint16_t port);

char read_serial(uint16_t port);

int is_transmit_empty(uint16_t port);
 
void write_serial(char a, uint16_t port);