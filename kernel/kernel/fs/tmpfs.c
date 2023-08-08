#include <kernel/kmalloc.h>
#include <kernel/list.h>

list_t* inodes;



void tmpfs_create_file(){
    struct tmpfs_file* file;
    char* file_content = kmalloc(sizeof(char) * 10);
}

void tmpfs_open(){}

void tmpfs_init(){
    inodes = list_create();
}
