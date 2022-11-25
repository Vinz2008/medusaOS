#include <kernel/vfs.h>

typedef struct Device
{
    char name[16];
    FileType device_type;
    read_block_type_t read_block;
    write_block_type_t write_block;
    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    void * private_data;
} Device;