#include <kernel/device.h>
#include <string.h>
#include <kernel/devfs.h>

void null_open(fs_node_t* node){
    (void)node;
}

void register_devices_necessary(){
    //Device stdin_dev = (Device){.name="stdin", .close=0, .read=0, .write=0,};
    Device stdin_dev;
    memset((uint8_t*)&stdin_dev, 0, sizeof(Device));
    strcpy(stdin_dev.name, "stdin");
    devfs_register_device(&stdin_dev);
    Device stdout_dev = (Device){.name="stdout", .close=0, .read=0, .write=0,};
    devfs_register_device(&stdout_dev);
    Device tty_dev = (Device){.name="tty", .close=0, .read=0, .write=0,};
    devfs_register_device(&tty_dev);
    Device null_dev;
    memset((uint8_t*)&null_dev, 0, sizeof(Device));
    strcpy(null_dev.name, "null");
    null_dev.open = null_open;
    devfs_register_device(&null_dev);
}