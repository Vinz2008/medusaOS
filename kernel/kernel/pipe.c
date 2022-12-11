#include <kernel/pipe.h>
#include <string.h>
#include <kernel/vfs.h>
#include <kernel/kmalloc.h>

fs_node_t* make_pipe(size_t size){
    fs_node_t* node = kmalloc(sizeof(fs_node_t));
    pipe_device_t* pipe = kmalloc(sizeof(pipe_device_t));
    node->inode = 0;
    strcpy(node->name, "[pipe]");
    node->uid = node->gid = 0;
    node->read  = 0;
	node->write = 0;
	node->open  = 0;
	node->close = 0;
	node->readdir = NULL;
	node->finddir = NULL;
    node->inode = (uintptr_t)pipe;
    pipe->buffer = kmalloc(size);
    pipe->write_ptr = 0;
    pipe->read_ptr = 0;
    pipe->size = size;
    pipe->refcount = 0;
    pipe->lock = 0;
    pipe->wait_queue = kmalloc(10); // will see later what type and size it needs to be
    return node;
}