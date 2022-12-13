#include <kernel/device.h>
#include <kernel/devfs.h>

void register_devices_necessary(){
    Device stdin_dev = (Device){.name="stdin", .close=0, .read=0, .write=0,};
    devfs_register_device(&stdin_dev);
    Device stdout_dev = (Device){.name="stdout", .close=0, .read=0, .write=0,};
    devfs_register_device(&stdout_dev);
    Device tty_dev = (Device){.name="tty", .close=0, .read=0, .write=0,};
    devfs_register_device(&tty_dev);
}