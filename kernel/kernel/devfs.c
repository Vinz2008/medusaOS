#include <kernel/devfs.h>
#include <string.h>
#include <stdio.h>
#include <kernel/kmalloc.h>
#include <kernel/vfs.h>
#include <kernel/list.h>


list_t* device_list;
//static fs_node_t* device_list = NULL;
//int n_devices = 0;
static void devfs_open(struct fs_node* node);
fs_node_t* dev_root = NULL;

extern bool initrd_readdir_call;


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
    log(LOG_SERIAL, false, "dev readdir: %d, nb devices : %d\n", index, device_list->used);
    if (index >= device_list->used){
        if (initrd_readdir_call){
            // put to 0 to reset index for if last call to readdir was to an other filesystem
            index = 0;
        } else {
            return NULL;
        }
    }
    initrd_readdir_call = false;
    for (int i = 0; i < device_list->used; i++){
        fs_node_t* temp_node = device_list->list[i].data;
        log(LOG_SERIAL, false, "readdir device_list[%d] : %s\n", i, temp_node->name);
        log(LOG_SERIAL, false, "readdir device_list[%d] ptr name : %p\n", i, &(temp_node->name));
    }
    fs_node_t* node_found = device_list->list[index].data;
    log(LOG_SERIAL, false, "name of device returned : %s\n", node_found->name);
    log(LOG_SERIAL, false, "name of device returned ptr : %p\n", node_found->name);
    return node_found;
}

/*void add_to_device_list(fs_node_t* node){
    //krealloc(device_list, sizeof(fs_node_t) * (n_devices + 1));
    device_list[n_devices] = *node;
    log(LOG_SERIAL, false, "device_list[%d] : %s\n", n_devices, device_list[n_devices].name);
    n_devices++;
}*/

fs_node_t* devfs_register_device(Device* device){
    for (int i = 0; i < device_list->used; i++){
        fs_node_t* temp_node = (fs_node_t*)device_list->list[i].data;
        if (strcmp(device->name, temp_node->name) == 0){
            return NULL;
        }
    }
    fs_node_t* device_node = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    memset((uint8_t*)device_node, 0, sizeof(fs_node_t));
    log(LOG_SERIAL, false, "device->name : %s\n", device->name);
    memset(device_node->name, 0, sizeof(device_node->name));
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
    list_append(device_node, device_list);
    //add_to_device_list(device_node);
    log(LOG_SERIAL, false, "after adding device_list[%d] : %s\n", device_list->used - 1, ((fs_node_t*)device_list->list[device_list->used - 1].data)->name);
    return device_node;
}


void devfs_initialize(){
    dev_root = kmalloc(sizeof(fs_node_t));
    memset((uint8_t*)dev_root, 0, sizeof(fs_node_t));
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
    device_list = list_create();
    //device_list = kmalloc(sizeof(fs_node_t));
}
