#include <string.h>
#include <kernel/list.h>
#include <kernel/kmalloc.h>

list_t* list_create(){
    list_t* list = kmalloc(sizeof(list_t));
    list->used = 0;
    list->list = kmalloc(sizeof(struct listItem)*1);
    list->size = 1;
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

/*List* list_create()
{
    List* list = (List*)kmalloc(sizeof(List));

    memset((uint8_t*)list, 0, sizeof(List));

    return list;
}

void list_clear(List* list)
{
    ListNode* list_node = list->head;

    while (NULL != list_node)
    {
        ListNode* next = list_node->next;

        kfree(list_node);

        list_node = next;
    }

    list->head = NULL;
    list->tail = NULL;
}

void list_destroy(List* list)
{
    list_clear(list);

    kfree(list);
}

List* list_create_clone(List* list)
{
    List* newList = list_create();

    list_foreach(n, list)
    {
        list_append(newList, n->data);
    }

    return newList;
}

bool list_is_empty(List* list)
{
    //At empty state, both head and tail are null!
    return list->head == NULL;
}

void list_append(List* list, void* data)
{
    ListNode* node = (ListNode*)kmalloc(sizeof(ListNode));

    memset((uint8_t*)node, 0, sizeof(ListNode));
    node->data = data;

    //At empty state, both head and tail are null!
    if (NULL == list->tail)
    {
        list->head = node;

        list->tail = node;

        return;
    }

    node->previous = list->tail;
    node->previous->next = node;
    list->tail = node;
}

void list_prepend(List* list, void* data)
{
    ListNode* node = (ListNode*)kmalloc(sizeof(ListNode));

    memset((uint8_t*)node, 0, sizeof(ListNode));
    node->data = data;

    //At empty state, both head and tail are null!
    if (NULL == list->tail)
    {
        list->head = node;

        list->tail = node;

        return;
    }

    node->next = list->head;
    node->next->previous = node;
    list->head = node;
}

ListNode* list_get_first_node(List* list)
{
    return list->head;
}

ListNode* list_get_last_node(List* list)
{
    return list->tail;
}*/