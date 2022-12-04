#include <kernel/devfs.h>
#include <string.h>
#include <stdio.h>
#include <kernel/kmalloc.h>
#include <kernel/vfs.h>
#include <kernel/list.h>


static fs_node_t* device_list = NULL;
int n_devices = 0;
static void devfs_open(struct fs_node* node);
fs_node_t* dev_root = NULL;


fs_node_t* get_root_devfs(){
    return dev_root;
}

void devfs_open(struct fs_node* node){

}

fs_node_t* devfs_finddir(struct fs_node* node, char *name){
    log(LOG_SERIAL, false, "dev finddir: %s\n", name);
    return NULL;
}

struct dirent* devfs_readdir(struct fs_node* node, uint32_t index){
    log(LOG_SERIAL, false, "dev readdir: %d, n_devices : %d\n", index, n_devices);
    if (index >= n_devices){
        return NULL;
    }
    for (int i = 0; i < n_devices; i++){
        log(LOG_SERIAL, false, "readdir device_list[%d] : %s\n", i, device_list[i].name);
    }
    return &device_list[index];
}

void add_to_device_list(fs_node_t* node){
    if (device_list == NULL){
        device_list = kmalloc(sizeof(fs_node_t) * 2);
    } else {
        krealloc(device_list, sizeof(fs_node_t) * n_devices + sizeof(fs_node_t));
    }
    device_list[n_devices] = *node;
    log(LOG_SERIAL, false, "device_list[%d] : %s\n", n_devices, device_list[n_devices].name);
    n_devices++;
}

fs_node_t* devfs_register_device(Device* device){
    for (int i = 0; i < n_devices; i++){
        if (strcmp(device->name, device_list[i].name) == 0){
            return NULL;
        }
    }
    fs_node_t* device_node = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    //memset((uint8_t*)device_node, 0, sizeof(fs_node_t));
    log(LOG_SERIAL, false, "device->name : %s\n", device->name);
    strcpy(device_node->name, device->name);
    device_node->node_type = device->device_type;
    device_node->open = device->open;
    device_node->close = device->close;
    device_node->read_block = device->read_block;
    device_node->write_block = device->write_block;
    device_node->read = device->read;
    device_node->write = device->write;
    device_node->private_node_data = device->private_data;
    device_node->parent = dev_root;
    add_to_device_list(device_node);
    log(LOG_SERIAL, false, "after adding device_list[%d] : %s\n", n_devices - 1, device_list[n_devices - 1].name);
    return device_node;
}


void devfs_initialize(){
    dev_root = kmalloc(sizeof(fs_node_t));
    //memset((uint8_t*)dev_root, 0, sizeof(fs_node_t));
    fs_node_t* root_node = fs_get_root_node();
    fs_node_t* dev_node = finddir_fs(root_node, "dev/");
    if (dev_node != NULL){
        dev_root->parent = dev_node->parent;
        strcpy(dev_root->name, dev_node->name);
    } else {
        log(LOG_SERIAL, false, "/dev doesn't exist\n");
        strcpy(dev_root->name, "dev/");
    }
    dev_root->mask = dev_root->uid = dev_root->gid = dev_root->inode = dev_root->length = 0;
    dev_root->flags = FS_DIRECTORY;
    dev_root->node_type = FT_Directory;
    dev_root->read = 0;
    dev_root->write = 0;
    dev_root->close = 0;
    dev_root->mount_point = 0;
    dev_root->impl = 0;
    dev_root->open = &devfs_open;
    dev_root->finddir = &devfs_finddir;
    dev_root->readdir = &devfs_readdir;
}

void register_devices_necessary(){
    Device stdout_dev = (Device){.name="stdout", .close=0, .read=0, .write=0,};
    devfs_register_device(&stdout_dev);
    Device stdin_dev = (Device){.name="stdin", .close=0, .read=0, .write=0,};
    devfs_register_device(&stdin_dev);
}