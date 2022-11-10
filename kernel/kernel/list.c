#include <kernel/list.h>
#include <kernel/kmalloc.h>

list_t* list_node_new(void* data) {
    list_t* node = (list_t*)kmalloc(sizeof(list_t));
    if (!node) {
        return NULL;
    }
    node->data = data;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

void __list_add(list_t* new, list_t* prev, list_t* next) {
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

list_t* list_add(list_t* list, void* data) {
    list_t* new = list_node_new(data);
    if (!new) {
        return NULL;
    }
    __list_add(new, list->prev, list);
    return list;
}

list_t* list_add_front(list_t* list, void* data) {
    list_t* new = list_node_new(data);
    if (!new) {
        return NULL;
    }
    __list_add(new, list, list->next);
    return list;
}

void __list_del(list_t* prev, list_t* next) {
    next->prev = prev;
    prev->next = next;
}

void list_del(list_t* entry) {
    __list_del(entry->prev, entry->next);
    entry->next = NULL; // Safety first, TODO: remove
    entry->prev = NULL;
    kfree(entry);
}

void list_move(list_t* list, list_t* head) {
    list_add_front(head, list->data);
    list_del(list);
}