#include <string.h>
#include <stdio.h>
#include <kernel/list.h>
#include <kernel/kmalloc.h>

list_t* list_create(){
    list_t* list = kmalloc(sizeof(list_t));
    list->used = 0;
    list->list = kmalloc(sizeof(struct listItem)*1);
    list->size = 1;
    return list;
}

list_t*  list_create_ext(size_t used, size_t size){
    list_t* list = kmalloc(sizeof(list_t));
    list->used = used;
    list->list = kmalloc(sizeof(struct listItem)*size);
    list->size = size;
    return list;
}

void list_clear(list_t* list){
    for (int i = 0; i < list->used; i++){
        list->list[i].data = NULL;
    }
}

void list_destroy(list_t* list){
    list_clear(list);
    kfree(list->list);
    kfree(list);
}

bool list_is_empty(list_t* list){
    return list->used == 0;
}

void list_append(void* data, list_t* list){
    if (list->used == list->size){
        list->size *= 2;
        list->list = krealloc(list->list, list->size * sizeof(struct listItem));
    }
    list->list[list->used++].data = data;
}
