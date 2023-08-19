#include <kernel/kmalloc.h>
#include <kernel/list.h>

list_t* inodes;

struct dirent* memfs_readdir(){
    
}


struct tmpfs_file* memfs_create_file(char* name){
    struct tmpfs_file* file;
    char* file_content = kmalloc(sizeof(char) * 10);
}

void tmpfs_open(){}

void tmpfs_init(){
    inodes = list_create();
}
