#include <kernel/initrd.h>
#include <stdio.h>
#include <stdbool.h>
#include <types.h>
#include <string.h>
#include <kernel/serial.h>
#include <kernel/kheap.h>
#include <kernel/vfs.h>

struct tar_header *headers[32];


initrd_header_t *initrd_header;     // The header.
fs_node_t *initrd_root;             // Our root directory node.
fs_node_t *initrd_dev;              // We also add a directory node for /dev, so we can mount devfs later on.
fs_node_t *root_nodes;              // List of file nodes.
int nroot_nodes;                    // Number of file nodes.

struct dirent dirent; 

bool isInitrdInitialized = false;;

unsigned int getsize(const char *in){
    unsigned int size = 0;
    unsigned int j;
    unsigned int count = 1; 
    for (j = 11; j > 0; j--, count *= 8)
        size += ((in[j - 1] - '0') * count);
    return size;
}

unsigned int initrd_get_number_files(unsigned int address){
    unsigned int i;
    for (i = 0; ; i++){
        struct tar_header *header = (struct tar_header *)address;
        if (header->filename[0] == '\0')
            break;
        write_serialf("filename %i : %s\n",i, header->filename);
        unsigned int size = getsize(header->size);
        headers[i] = header;
        address += ((size / 512) + 1) * 512;
        if (size % 512)
            address += 512;
    }
    return i;
}


void initrd_list_filenames(unsigned int address){
    unsigned int i = 0;
    while (i <= initrd_get_number_files(address)){
        struct tar_header *header = (struct tar_header *)address;
        if (header->filename[0] == '\0')
            break;
        printf("\nfilename [%i] : %s\n",i, header->filename);
        unsigned int size = getsize(header->size);
        headers[i] = header;
        address += ((size / 512) + 1) * 512;
        if (size % 512)
            address += 512;
        i++;
    }
}

fs_node_t *initialise_initrd(uint32_t location){
    initrd_header->nfiles = initrd_get_number_files(location);
    initrd_root = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    strcpy(initrd_root->name, "initrd");
    initrd_root->mask = initrd_root->uid = initrd_root->gid = initrd_root->inode = initrd_root->length = 0;
    initrd_root->flags = FS_DIRECTORY;
    initrd_root->read = 0;
    initrd_root->write = 0;
    initrd_root->open = 0;
    initrd_root->close = 0;
    /*initrd_root->readdir = &initrd_readdir;
    initrd_root->finddir = &initrd_finddir;*/
    initrd_root->readdir = 0;
    initrd_root->finddir = 0;
    initrd_dev = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    strcpy(initrd_dev->name, "dev");
    initrd_dev->mask = initrd_dev->uid = initrd_dev->gid = initrd_dev->inode = initrd_dev->length = 0;
    initrd_dev->flags = FS_DIRECTORY;
    initrd_dev->read = 0;
    initrd_dev->write = 0;
    initrd_dev->open = 0;
    initrd_dev->close = 0;
    /*initrd_dev->readdir = &initrd_readdir;
    initrd_dev->finddir = &initrd_finddir;*/
    initrd_dev->readdir = 0;
    initrd_dev->finddir = 0;
    initrd_dev->ptr = 0;
    initrd_dev->impl = 0;
    root_nodes = (fs_node_t*)kmalloc(sizeof(fs_node_t) * initrd_header->nfiles);
    nroot_nodes = initrd_header->nfiles;
    isInitrdInitialized = true;
}