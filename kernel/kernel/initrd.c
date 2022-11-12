#include <kernel/initrd.h>
#include <stdio.h>
#include <stdbool.h>
#include <types.h>
#include <string.h>
#include <kernel/serial.h>
#include <kernel/kmalloc.h>
#include <kernel/vfs.h>

struct tar_header *headers[32];

initrd_header_t *initrd_header;     // The header.
fs_node_t *initrd_root;             // Our root directory node.
fs_node_t *initrd_dev;              // We also add a directory node for /dev, so we can mount devfs later on.
fs_node_t *root_nodes;              // List of file nodes.
int nroot_nodes;  
initrd_file_header_t *file_headers; // Number of file nodes.

struct dirent dirent; 

bool isInitrdInitialized = false;

static uint32_t initrd_adress;

void set_initrd_address(uint32_t addr){
    initrd_adress = addr;
}

uint32_t get_initrd_address(){
    return initrd_adress;
}


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

static uint32_t initrd_read(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer){
	initrd_file_header_t header = file_headers[node->inode];
	if (offset > header.length)
		return 0;
	if (offset + size > header.length)
		size = header.length - offset;
	memcpy(buffer, (uint8_t*) (header.offset + offset), size);
	return size;
}

static struct dirent *initrd_readdir(fs_node_t *node, uint32_t index){
	if (node == initrd_root && index == 0) {
		strcpy(dirent.name, "dev");
		dirent.name[3] = 0;
		dirent.ino = 0;
		return &dirent;
	}

	if (index-1 >= nroot_nodes)
		return 0;

	strcpy(dirent.name, root_nodes[index-1].name);
	dirent.name[strlen(root_nodes[index-1].name)] = 0;
	dirent.ino = root_nodes[index-1].inode;
	return &dirent;
}

static fs_node_t *initrd_finddir(fs_node_t *node, char *name){
	if (node == initrd_root && !strcmp(name, "dev") )
		return initrd_dev;

	int i;
	for (i = 0; i < nroot_nodes; i++)
		if (!strcmp(name, root_nodes[i].name))
			return &root_nodes[i];
	return 0;
}

fs_node_t *initialise_initrd(uint32_t location){
    //initrd_header = (initrd_header_t*) location;
    file_headers = (initrd_file_header_t*) (location + sizeof(initrd_header_t));

    initrd_header->nfiles = initrd_get_number_files(location);
    log(LOG_SERIAL, false, "nb_files: %d\n", initrd_header->nfiles);
    initrd_root = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    strcpy(initrd_root->name, "initrd");
    initrd_root->mask = initrd_root->uid = initrd_root->gid = initrd_root->inode = initrd_root->length = 0;
    initrd_root->flags = FS_DIRECTORY;
    initrd_root->read = 0;
    initrd_root->write = 0;
    initrd_root->open = 0;
    initrd_root->close = 0;
    initrd_root->readdir = &initrd_readdir;
    initrd_root->finddir = &initrd_finddir;
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
    initrd_dev->readdir = &initrd_readdir;
    initrd_dev->finddir = &initrd_finddir;
    initrd_dev->readdir = 0;
    initrd_dev->finddir = 0;
    initrd_dev->ptr = 0;
    initrd_dev->impl = 0;
    root_nodes = (fs_node_t*)kmalloc(sizeof(fs_node_t) * initrd_header->nfiles);
    nroot_nodes = initrd_header->nfiles;
    for (int i = 0; i < initrd_header->nfiles; i++){
        file_headers[i].offset += location;
        strcpy(root_nodes[i].name, &file_headers[i].name);
		root_nodes[i].mask	= root_nodes[i].uid = root_nodes[i].gid = 0;
		root_nodes[i].length = file_headers[i].length;
		root_nodes[i].inode	= i;
		root_nodes[i].flags	= FS_FILE;
		root_nodes[i].read	= &initrd_read;
		root_nodes[i].write	= 0;
		root_nodes[i].readdir	= 0;
		root_nodes[i].finddir	= 0;
		root_nodes[i].open	= 0;
		root_nodes[i].close	= 0;
		root_nodes[i].impl	= 0;
    }


    isInitrdInitialized = true;
    log(LOG_SERIAL, false, "initrd initialized\n");
    return initrd_root;
}