#include <kernel/devfs.h>
#include <string.h>
#include <stdio.h>
#include <kernel/kmalloc.h>
#include <kernel/vfs.h>
#include <kernel/device.h>
#include <kernel/list.h>


static List* g_device_list = NULL;
static void devfs_open(struct fs_node* node);
static fs_node_t* devfs_finddir(struct fs_node* node, char *name);
static struct dirent* devfs_readdir(struct fs_node* node, uint32_t index);
static fs_node_t* g_dev_root = NULL;

void devfs_initialize(){
    g_dev_root = kmalloc(sizeof(fs_node_t));
    memset((uint8_t*)g_dev_root, 0, sizeof(fs_node_t));
    fs_node_t* root_node = fs_get_root_node();
    fs_node_t* dev_node = finddir_fs(root_node, "dev");
    if (dev_node){
        g_dev_root->parent = dev_node->parent;
        strcpy(g_dev_root->name, dev_node->name);
    } else {
        log(LOG_SERIAL, false, "/dev doesn't exist\n");
    }
    g_dev_root->open = devfs_open;
    g_dev_root->finddir = devfs_finddir;
    g_dev_root->readdir = devfs_readdir;
}

void devfs_open(struct fs_node* node){

}

fs_node_t* devfs_finddir(struct fs_node* node, char *name){

}

struct dirent* devfs_readdir(struct fs_node* node, uint32_t index){

}

fs_node_t* devfs_register_device(Device* device){
    list_foreach(n, g_device_list){
        fs_node_t* device_node = (fs_node_t*)n->data;
        if (strcmp(device->name, device_node->name) == 0){
            return NULL;
        }
    }
    fs_node_t* device_node = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    memset((uint8_t*)device_node, 0, sizeof(fs_node_t));
    strcpy(device_node->name, device->name);
    device_node->node_type = device->device_type;
    device_node->open = device->open;
    device_node->close = device->close;
    device_node->read_block = device->read_block;
    device_node->write_block = device->write_block;
    device_node->read = device->read;
    device_node->write = device->write;
    device_node->private_node_data = device->private_data;
    device_node->parent = g_dev_root;
    list_append(g_device_list, device_node);
    return device_node;
}

